#include "EndSceneState.h"
#include "GlyphHelper.h"
#include "Components/SceneStateMachineComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCodes.h"
#include "Input/ControllerButtons.h"
#include "Rendering/Renderer.h"
#include "Commands/Command.h"
#include "Managers/HighScoreManager.h"
#include "Resources/ResourceManager.h"
#include "Commands/ToggleMuteCommand.h"
#include "State/TitleSceneState.h"
#include "Core/GameTime.h"
#include <iomanip>
#include <sstream>

namespace
{
	constexpr float kScoreStartX{ 65.0f };
	constexpr float kScoreRow1Y{ 172.0f };
	constexpr float kScoreRow2Y{ 188.0f };
	constexpr float kScoreRow3Y{ 204.0f };
	constexpr float kScoreRow4Y{ 220.0f };
	constexpr float kNameStartX{ 145.0f };
	constexpr float kReturnArrowX{ 195.0f };
	constexpr float kReturnArrowY{ 225.0f };
}

namespace dae
{
	namespace
	{
		class EndSceneCommand final : public Command
		{
		public:
			enum class Action
			{
				LetterUp,
				LetterDown,
				Confirm,
			};

			EndSceneCommand(EndSceneState& state, Action action)
				: m_State(state)
				, m_Action(action)
			{}

			void Execute() override
			{
				switch (m_Action)
				{
				case Action::LetterUp:
					m_State.MoveLetter(-1);
					break;
				case Action::LetterDown:
					m_State.MoveLetter(1);
					break;
				case Action::Confirm:
					m_State.ConfirmLetter();
					break;
				}
			}

		private:
			EndSceneState& m_State;
			Action m_Action;
		};

		class ReturnCommand final : public Command
		{
		public:
			explicit ReturnCommand(EndSceneState& state)
				: m_State(state)
			{}

			void Execute() override
			{
				m_State.ConfirmReturn();
			}

		private:
			EndSceneState& m_State;
		};
	}

	EndSceneState::EndSceneState(SceneStateMachineComponent& owner,
		int player1Score, int player2Score, bool hasTwoPlayers)
		: SceneState(owner)
	{
		m_PlayerScores[0] = player1Score;
		m_PlayerScores[1] = player2Score;
		m_HasTwoPlayers = hasTwoPlayers;
		m_CurrentName[0] = 'A';
		m_CurrentName[1] = 'A';
		m_CurrentName[2] = 'A';
		m_CurrentName[3] = 'A';
		m_CurrentName[4] = '\0';
	}

	void EndSceneState::OnEnter()
	{
		BuildScene();
		BindInput();
	}

	void EndSceneState::OnExit()
	{
		UnbindInput();
		ClearScene();
	}

	void EndSceneState::Update()
	{
		if (m_Phase == Phase::CheckScores)
		{
			AdvancePhase();
			return;
		}

		if (m_Phase == Phase::EnterName && m_pCurrentLetterRender)
		{
			m_BlinkTimer += GameTime::GetInstance().GetDeltaTime();
			if (m_BlinkTimer >= 0.25f)
			{
				m_BlinkTimer = 0.0f;
				m_IndicatorVisible = !m_IndicatorVisible;
				const float glyphSize = kGlyphWidth * m_Scale;
				m_pCurrentLetterRender->SetDestinationSize(m_IndicatorVisible ? glyphSize : 0.0f, glyphSize);
			}
		}
	}

	void EndSceneState::AdvancePhase()
	{
		m_PendingEntries.clear();
		m_CurrentPendingIndex = 0;

		int scores[2] = { m_PlayerScores[0], m_PlayerScores[1] };
		int indices[2] = { 0, 1 };

		if (m_HasTwoPlayers && scores[0] < scores[1])
		{
			std::swap(scores[0], scores[1]);
			std::swap(indices[0], indices[1]);
		}

		bool anyQualify = false;
		bool qualifyMask[2] = { false, false };
		for (int i = 0; i < (m_HasTwoPlayers ? 2 : 1); ++i)
		{
			if (scores[i] > 0 && HighScoreManager::IsHighScore(scores[i]))
			{
				anyQualify = true;
				qualifyMask[i] = true;
			}
		}

		if (anyQualify)
		{
			for (int i = 0; i < (m_HasTwoPlayers ? 2 : 1); ++i)
			{
				if (!qualifyMask[i])
					continue;

				const std::string placeholder = (indices[i] == 0) ? "AAAA" : "BBBB";
				const int entryIndex = HighScoreManager::SubmitScore(placeholder, scores[i]);
				if (entryIndex >= 0)
					m_PendingEntries.push_back({ scores[i], indices[i], entryIndex });
			}
		}

		if (!m_PendingEntries.empty())
		{
			m_Phase = Phase::EnterName;
			m_CurrentCharIndex = 0;
			m_CurrentName[0] = 'A';
			m_CurrentName[1] = 'A';
			m_CurrentName[2] = 'A';
			m_CurrentName[3] = 'A';
			m_CurrentName[4] = '\0';
			RefreshScoreDisplay();
			RefreshNameDisplay();

			UnbindInput();
			BindInput(m_PendingEntries[0].playerIndex);
		}
		else
		{
			m_Phase = Phase::WaitForReturn;
			m_ReturnActive = true;

			RefreshScoreDisplay();
			RefreshNameDisplay();

			if (m_pReturnArrowRender)
				m_pReturnArrowRender->SetDestinationSize(8.0f * m_Scale, 8.0f * m_Scale);

			UnbindInput();
			BindInput();
		}
	}

	void EndSceneState::SubmitCurrentName()
	{
		if (m_CurrentPendingIndex >= static_cast<int>(m_PendingEntries.size()))
			return;

		const int entryIndex = m_PendingEntries[m_CurrentPendingIndex].entryIndex;
		HighScoreManager::UpdateName(entryIndex, m_CurrentName);
		HighScoreManager::Save((ResourceManager::GetInstance().GetDataPath() / "highscores.bin").string());
		RefreshScoreDisplay();
		RefreshNameDisplay();
	}

	void EndSceneState::MoveLetter(int direction)
	{
		if (m_Phase != Phase::EnterName)
			return;

		m_CurrentName[m_CurrentCharIndex] = CycleLetter(m_CurrentName[m_CurrentCharIndex], direction);
		RefreshNameDisplay();
	}

	void EndSceneState::ConfirmLetter()
	{
		if (m_Phase == Phase::WaitForReturn)
		{
			ConfirmReturn();
			return;
		}

		if (m_Phase != Phase::EnterName)
			return;

		++m_CurrentCharIndex;
		if (m_CurrentCharIndex >= kNameLength)
		{
			SubmitCurrentName();
			++m_CurrentPendingIndex;

			if (m_CurrentPendingIndex < static_cast<int>(m_PendingEntries.size()))
			{
				m_CurrentCharIndex = 0;
				m_CurrentName[0] = 'A';
				m_CurrentName[1] = 'A';
				m_CurrentName[2] = 'A';
				m_CurrentName[3] = 'A';
				m_CurrentName[4] = '\0';
				RefreshNameDisplay();

				UnbindInput();
				BindInput(m_PendingEntries[m_CurrentPendingIndex].playerIndex);
			}
			else
			{
				m_Phase = Phase::WaitForReturn;
				m_ReturnActive = true;

				if (m_pReturnArrowRender)
					m_pReturnArrowRender->SetDestinationSize(8.0f * m_Scale, 8.0f * m_Scale);

				UnbindInput();
				BindInput();
			}
		}
		else
		{
			m_CurrentName[m_CurrentCharIndex] = 'A';
			RefreshNameDisplay();
		}
	}

	void EndSceneState::ConfirmReturn()
	{
		m_Owner.GetStateMachine().SetState(
			std::make_unique<TitleSceneState>(m_Owner)
		);
	}

	void EndSceneState::BuildScene()
	{
		auto& scene = SceneManager::GetInstance().CreateScene();
		m_pScene = &scene;
		m_Owner.SetActiveScene(scene);

		const auto windowSize = Renderer::GetInstance().GetWindowSize();
		const float windowWidth = static_cast<float>(windowSize.x);
		const float windowHeight = static_cast<float>(windowSize.y);
		m_Scale = windowHeight / kBackgroundHeight;
		const float scaledWidth = kBackgroundWidth * m_Scale;
		const float scaledHeight = kBackgroundHeight * m_Scale;
		m_OffsetX = (windowWidth - scaledWidth) * 0.5f;
		m_OffsetY = (windowHeight - scaledHeight) * 0.5f;

		auto rootObject = std::make_unique<GameObject>();
		m_pRoot = rootObject.get();
		auto* rootTransform = m_pRoot->AddComponent<TransformComponent>();
		rootTransform->SetLocalPosition(m_OffsetX, m_OffsetY);
		scene.Add(std::move(rootObject));

		auto background = std::make_unique<GameObject>();
		background->AddComponent<TransformComponent>();
		auto* bgRender = background->AddComponent<RenderComponent>();
		bgRender->SetTexture("BombermanSprites_TitleScreen.png");
		bgRender->SetSourceRectangle(kBackgroundSrcX, kBackgroundSrcY, kBackgroundWidth, kBackgroundHeight);
		bgRender->SetDestinationSize(scaledWidth, scaledHeight);
		bgRender->SetRenderLayer(kBackgroundLayer);
		background->SetParent(m_pRoot, false);
		scene.Add(std::move(background));

		auto returnArrow = std::make_unique<GameObject>();
		auto* returnTransform = returnArrow->AddComponent<TransformComponent>();
		returnTransform->SetLocalPosition(kReturnArrowX * m_Scale, kReturnArrowY * m_Scale);
		m_pReturnArrowRender = returnArrow->AddComponent<RenderComponent>();
		m_pReturnArrowRender->SetTexture("BombermanSprites_TitleScreen.png");
		m_pReturnArrowRender->SetSourceRectangle(kArrowGlyphX, kGlyphRowY, kGlyphWidth, kGlyphHeight);
		m_pReturnArrowRender->SetDestinationSize(0.0f, 0.0f);
		m_pReturnArrowRender->SetRenderLayer(kReturnArrowLayer);
		returnArrow->SetParent(m_pRoot, false);
		m_pReturnArrow = returnArrow.get();
		scene.Add(std::move(returnArrow));

		auto textRoot = std::make_unique<GameObject>();
		textRoot->AddComponent<TransformComponent>();
		textRoot->SetParent(m_pRoot, false);
		m_pTextRoot = textRoot.get();
		scene.Add(std::move(textRoot));
	}

	void EndSceneState::RefreshScoreDisplay()
	{
		RebuildTextDisplay();
	}

	void EndSceneState::RefreshNameDisplay()
	{
		RebuildTextDisplay();
	}

	void EndSceneState::RebuildTextDisplay()
	{
		if (!m_pScene || !m_pRoot || !m_pTextRoot)
			return;

		m_pTextRoot->MarkForDeletion();
		m_pTextRoot = nullptr;

		auto textRoot = std::make_unique<GameObject>();
		textRoot->AddComponent<TransformComponent>();
		textRoot->SetParent(m_pRoot, false);
		m_pTextRoot = textRoot.get();
		m_pScene->Add(std::move(textRoot));

		m_pCurrentLetterRender = nullptr;

		const auto& entries = HighScoreManager::GetEntries();
		const float rowYs[HighScoreManager::kMaxEntries] = { kScoreRow1Y, kScoreRow2Y, kScoreRow3Y, kScoreRow4Y };
		const int pendingEntryIndex = (m_Phase == Phase::EnterName && m_CurrentPendingIndex < static_cast<int>(m_PendingEntries.size()))
			? m_PendingEntries[m_CurrentPendingIndex].entryIndex : -1;

		for (int i = 0; i < HighScoreManager::kMaxEntries; ++i)
		{
			const std::string scoreStr = ScoreToString(entries[i].score);
			float cursorX = kScoreStartX * m_Scale;
			const float cursorY = rowYs[i] * m_Scale;
			const float glyphSize = kGlyphWidth * m_Scale;

			for (char c : scoreStr)
			{
				CreateGlyph(*m_pScene, *m_pTextRoot, c, cursorX, cursorY, kTextLayer);
				cursorX += glyphSize;
			}
		}

		for (int i = 0; i < HighScoreManager::kMaxEntries; ++i)
		{
			std::string nameStr;
			if (i == pendingEntryIndex)
			{
				nameStr = m_CurrentName;
			}
			else
			{
				nameStr = entries[i].name;
			}

			float cursorX = kNameStartX * m_Scale;
			const float cursorY = rowYs[i] * m_Scale;
			const float glyphSize = kGlyphWidth * m_Scale;

			int charIndex = 0;
			for (char c : nameStr)
			{
				if (c == '\0')
					break;
				auto* render = CreateGlyph(*m_pScene, *m_pTextRoot, c, cursorX, cursorY, kTextLayer);
				if (i == pendingEntryIndex && charIndex == m_CurrentCharIndex)
				{
					m_pCurrentLetterRender = render;
				}
				cursorX += glyphSize;
				++charIndex;
			}
		}
	}

	void EndSceneState::ClearScene()
	{
		m_pRoot = nullptr;
		m_pTextRoot = nullptr;
		m_pCurrentLetterRender = nullptr;
		m_pReturnArrow = nullptr;
		m_pReturnArrowRender = nullptr;

		if (m_pScene)
		{
			m_pScene->RemoveAll();
			m_pScene = nullptr;
		}
	}

	void EndSceneState::BindInput(int playerIndex)
	{
		auto& input = InputManager::GetInstance();

		auto mute = std::make_unique<ToggleMuteCommand>();
		mute->SetGameActor(m_pRoot);
		input.BindKeyboardInput(KeyCode::F2, KeyState::Down, std::move(mute));

		if (m_Phase == Phase::CheckScores || m_Phase == Phase::EnterName)
		{
			if (playerIndex == 0 || playerIndex == -1)
			{
				input.BindKeyboardInput(KeyCode::Up, KeyState::Down, std::make_unique<EndSceneCommand>(*this, EndSceneCommand::Action::LetterUp));
				input.BindKeyboardInput(KeyCode::Down, KeyState::Down, std::make_unique<EndSceneCommand>(*this, EndSceneCommand::Action::LetterDown));
				input.BindKeyboardInput(KeyCode::Return, KeyState::Down, std::make_unique<EndSceneCommand>(*this, EndSceneCommand::Action::Confirm));
			}

			if (playerIndex == 1 || playerIndex == -1)
			{
				const unsigned int controllerIndex = 0;
				input.AddController(controllerIndex);
				input.BindControllerInput(controllerIndex, ControllerButton::kDpadUp, KeyState::Down, std::make_unique<EndSceneCommand>(*this, EndSceneCommand::Action::LetterUp));
				input.BindControllerInput(controllerIndex, ControllerButton::kDpadDown, KeyState::Down, std::make_unique<EndSceneCommand>(*this, EndSceneCommand::Action::LetterDown));
				input.BindControllerInput(controllerIndex, ControllerButton::kX, KeyState::Down, std::make_unique<EndSceneCommand>(*this, EndSceneCommand::Action::Confirm));
			}
		}
		else if (m_Phase == Phase::WaitForReturn)
		{
			input.BindKeyboardInput(KeyCode::Return, KeyState::Down, std::make_unique<ReturnCommand>(*this));

			const unsigned int controllerIndex = 0;
			input.AddController(controllerIndex);
			input.BindControllerInput(controllerIndex, ControllerButton::kX, KeyState::Down, std::make_unique<ReturnCommand>(*this));
		}
	}

	void EndSceneState::UnbindInput()
	{
		auto& input = InputManager::GetInstance();
		input.ClearAllBindings();
	}

	RenderComponent* EndSceneState::CreateGlyph(Scene& scene, GameObject& parent, char c, float localX, float localY, int layer)
	{
		auto [glyphX, glyphY] = GetCharacterSrcRect(c);

		auto glyph = std::make_unique<GameObject>();
		auto* transform = glyph->AddComponent<TransformComponent>();
		transform->SetLocalPosition(localX, localY);
		auto* render = glyph->AddComponent<RenderComponent>();
		render->SetTexture("BombermanSprites_TitleScreen.png");
		render->SetSourceRectangle(glyphX, glyphY, kGlyphWidth, kGlyphHeight);
		render->SetDestinationSize(kGlyphWidth * m_Scale, kGlyphHeight * m_Scale);
		render->SetRenderLayer(layer);
		glyph->SetParent(&parent, false);
		scene.Add(std::move(glyph));
		return render;
	}

	std::pair<float, float> EndSceneState::GetCharacterSrcRect(char character) const
	{
		return GlyphHelper::GetCharacterSrcRect(character);
	}

	char EndSceneState::CycleLetter(char current, int direction) const
	{
		if (current < 'A' || current > 'Z')
			current = 'A';

		int index = current - 'A';
		index = (index + direction + 26) % 26;
		return static_cast<char>('A' + index);
	}

	std::string EndSceneState::ScoreToString(int score) const
	{
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(kScoreDigits) << score;
		return oss.str();
	}
}
