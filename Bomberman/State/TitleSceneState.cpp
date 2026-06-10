#include "TitleSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Input/InputManager.h"
#include "Input/KeyCodes.h"
#include "Input/ControllerButtons.h"
#include "Rendering/Renderer.h"
#include "State/GameSceneState.h"
#include "State/TransitionSceneState.h"
#include "Commands/Command.h"
#include "GameMode.h"
#include "HighScoreManager.h"
#include <array>
#include <iomanip>
#include <sstream>
#include <utility>

namespace
{
	constexpr float kTitleSpriteWidth{ 256.0f };
	constexpr float kTitleSpriteHeight{ 240.0f };
	constexpr float kStartArrowX{ 63.0f };
	constexpr float kStartArrowY{ 152.0f };
	constexpr float kModeArrowX{ 127.0f };
	constexpr float kModeArrowY{ 152.0f };
	constexpr float kModeLabelX{ 177.0f };
	constexpr float kModeLabelY{ 153.0f };
	constexpr float kScoreX{ 113.0f };
	constexpr float kScoreY{ 169.0f };
	constexpr float kGlyphWidth{ 8.0f };
	constexpr float kGlyphHeight{ 8.0f };
	constexpr float kGlyphRowY{ 264.0f };
	constexpr float kGlyphRow2Y{ 272.0f };
	constexpr float kArrowGlyphX{ 80.0f };
	constexpr float kScoreWidth{ 72.0f };
	constexpr int kMenuLayer{ 2 };
	constexpr int kBackgroundLayer{ 0 };
	constexpr int kMenuOptions{ 2 };

	constexpr std::array<const char*, 3> kModeLabels{ "SOLO", "COOP", "1vs1" };
	constexpr int kModeCount{ 3 };
}

namespace dae
{
	namespace
	{
		class TitleMenuCommand final : public Command
		{
		public:
			enum class Action
			{
				Up,
				Down,
				Confirm,
				CycleMode
			};

			TitleMenuCommand(TitleSceneState& state, Action action)
				: m_State(state)
				, m_Action(action)
			{}

			void Execute() override
			{
				switch (m_Action)
				{
				case Action::Up:
					m_State.MoveSelection(-1);
					break;
				case Action::Down:
					m_State.MoveSelection(1);
					break;
				case Action::Confirm:
					m_State.ConfirmSelection();
					break;
				case Action::CycleMode:
					m_State.CycleMode();
					break;
				}
			}

		private:
			TitleSceneState& m_State;
			Action m_Action;
		};
	}

	TitleSceneState::TitleSceneState(SceneStateMachineComponent& owner)
		: SceneState(owner)
	{}

	void TitleSceneState::OnEnter()
	{
		BuildScene();
		UpdateMenuSprites();
		UpdateModeLabel();
		BindInput();
	}

	void TitleSceneState::OnExit()
	{
		UnbindInput();
		ClearScene();
	}

	void TitleSceneState::BuildScene()
	{
		auto* scene = m_Owner.GetActiveScene();
		if (!scene)
		{
			auto& newScene = SceneManager::GetInstance().CreateScene();
			m_Owner.SetActiveScene(newScene);
			scene = m_Owner.GetActiveScene();
		}
		if (!scene)
			return;

		const auto windowSize = Renderer::GetInstance().GetWindowSize();
		m_ScaleY = static_cast<float>(windowSize.y) / kTitleSpriteHeight;
		m_ScaleX = static_cast<float>(windowSize.x) / kTitleSpriteWidth;
		const float scale = m_ScaleY;
		const float scaledWidth = kTitleSpriteWidth * scale;
		const float scaledHeight = kTitleSpriteHeight * scale;
		const float offsetX = (static_cast<float>(windowSize.x) - scaledWidth) * 0.5f;
		const float offsetY = (static_cast<float>(windowSize.y) - scaledHeight) * 0.5f;
		m_ScaleX = scale;
		m_ArrowWidth = kGlyphWidth * m_ScaleX;
		m_ArrowHeight = kGlyphHeight * m_ScaleY;

		auto rootObject = std::make_unique<GameObject>();
		m_Root = rootObject.get();
		auto rootTransform = m_Root->AddComponent<TransformComponent>();
		rootTransform->SetLocalPosition(offsetX, offsetY);
		scene->Add(std::move(rootObject));

		auto background = std::make_unique<GameObject>();
		auto backgroundTransform = background->AddComponent<TransformComponent>();
		backgroundTransform->SetLocalPosition(0, 0);
		auto backgroundRender = background->AddComponent<RenderComponent>();
		backgroundRender->SetTexture("BombermanSprites_TitleScreen.png");
		backgroundRender->SetSourceRectangle(0, 0, kTitleSpriteWidth, kTitleSpriteHeight);
		backgroundRender->SetDestinationSize(scaledWidth, scaledHeight);
		backgroundRender->SetRenderLayer(kBackgroundLayer);
		background->SetParent(m_Root, false);
		scene->Add(std::move(background));

		auto menuRoot = std::make_unique<GameObject>();
		menuRoot->AddComponent<TransformComponent>();
		menuRoot->SetParent(m_Root, false);
		m_MenuRoot = menuRoot.get();
		scene->Add(std::move(menuRoot));

		auto startArrow = std::make_unique<GameObject>();
		auto startTransform = startArrow->AddComponent<TransformComponent>();
		startTransform->SetLocalPosition(kStartArrowX * m_ScaleX, kStartArrowY * m_ScaleY);
		m_StartArrow = startArrow->AddComponent<RenderComponent>();
		m_StartArrow->SetTexture("BombermanSprites_TitleScreen.png");
		m_StartArrow->SetSourceRectangle(kArrowGlyphX, kGlyphRowY, kGlyphWidth, kGlyphHeight);
		m_StartArrow->SetDestinationSize(m_ArrowWidth, m_ArrowHeight);
		m_StartArrow->SetRenderLayer(kMenuLayer);
		startArrow->SetParent(m_MenuRoot, false);
		scene->Add(std::move(startArrow));

		auto modeArrow = std::make_unique<GameObject>();
		auto modeTransform = modeArrow->AddComponent<TransformComponent>();
		modeTransform->SetLocalPosition(kModeArrowX * m_ScaleX, kModeArrowY * m_ScaleY);
		m_ModeArrow = modeArrow->AddComponent<RenderComponent>();
		m_ModeArrow->SetTexture("BombermanSprites_TitleScreen.png");
		m_ModeArrow->SetSourceRectangle(kArrowGlyphX, kGlyphRowY, kGlyphWidth, kGlyphHeight);
		m_ModeArrow->SetDestinationSize(m_ArrowWidth, m_ArrowHeight);
		m_ModeArrow->SetRenderLayer(kMenuLayer);
		modeArrow->SetParent(m_MenuRoot, false);
		scene->Add(std::move(modeArrow));

		auto modeLabelRoot = std::make_unique<GameObject>();
		auto modeLabelTransform = modeLabelRoot->AddComponent<TransformComponent>();
		modeLabelTransform->SetLocalPosition(kModeLabelX * m_ScaleX, kModeLabelY * m_ScaleY);
		modeLabelRoot->SetParent(m_MenuRoot, false);
		m_ModeLabelRoot = modeLabelRoot.get();
		scene->Add(std::move(modeLabelRoot));

		auto scoreRoot = std::make_unique<GameObject>();
		auto scoreTransform = scoreRoot->AddComponent<TransformComponent>();
		scoreTransform->SetLocalPosition(kScoreX * m_ScaleX, kScoreY * m_ScaleY);
		scoreRoot->SetParent(m_MenuRoot, false);
		m_ScoreRoot = scoreRoot.get();
		scene->Add(std::move(scoreRoot));

		UpdateModeLabel();

		const int topScore = HighScoreManager::GetEntries()[0].score;
		std::ostringstream scoreStream;
		scoreStream << std::setfill('0') << std::setw(5) << topScore;
		CreateTextSprites(*scene, *m_ScoreRoot, scoreStream.str(), (kScoreWidth - (5.0f * kGlyphWidth)) * m_ScaleX, 0.0f, kMenuLayer);
	}

	void TitleSceneState::UpdateMenuSprites()
	{
		if (m_StartArrow)
		{
			const float width = (m_SelectedIndex == 0) ? m_ArrowWidth : 0.0f;
			m_StartArrow->SetDestinationSize(width, m_ArrowHeight);
		}
		if (m_ModeArrow)
		{
			const float width = (m_SelectedIndex == 1) ? m_ArrowWidth : 0.0f;
			m_ModeArrow->SetDestinationSize(width, m_ArrowHeight);
		}
	}

	void TitleSceneState::UpdateModeLabel()
	{
		if (!m_ModeLabelRoot)
			return;

		m_ModeLabelRoot->MarkForDeletion();
		m_ModeLabelRoot = nullptr;

		auto* scene = m_Owner.GetActiveScene();
		if (!scene)
			return;
		auto labelRoot = std::make_unique<GameObject>();
		auto labelTransform = labelRoot->AddComponent<TransformComponent>();
		labelTransform->SetLocalPosition(kModeLabelX * m_ScaleX, kModeLabelY * m_ScaleY);
		labelRoot->SetParent(m_MenuRoot, false);
		m_ModeLabelRoot = labelRoot.get();
		scene->Add(std::move(labelRoot));

		CreateTextSprites(*scene, *m_ModeLabelRoot, kModeLabels[m_ModeIndex], 0.0f, 0.0f, kMenuLayer);
	}

	void TitleSceneState::CreateTextSprites(Scene& scene, GameObject& parent, const std::string& text, float x, float y, int renderLayer)
	{
		float cursorX = x;

		for (char c : text)
		{
			if (c == ' ')
			{
				cursorX += kGlyphWidth * m_ScaleX;
				continue;
			}
			auto [glyphX, glyphY] = GetCharacterSrcRect(c);

			auto glyph = std::make_unique<GameObject>();
			auto glyphTransform = glyph->AddComponent<TransformComponent>();
			glyphTransform->SetLocalPosition(cursorX, y);
			auto glyphRender = glyph->AddComponent<RenderComponent>();
			glyphRender->SetTexture("BombermanSprites_TitleScreen.png");
			glyphRender->SetSourceRectangle(glyphX, glyphY, kGlyphWidth, kGlyphHeight);
			glyphRender->SetDestinationSize(kGlyphWidth * m_ScaleX, kGlyphHeight * m_ScaleY);
			glyphRender->SetRenderLayer(renderLayer);
			glyph->SetParent(&parent, false);
			scene.Add(std::move(glyph));

			cursorX += kGlyphWidth * m_ScaleX;
		}
	}

	void TitleSceneState::ClearScene()
	{
		if (auto* scene = m_Owner.GetActiveScene())
		{
			scene->RemoveAll();
		}

		m_Root = nullptr;
		m_MenuRoot = nullptr;
		m_StartArrow = nullptr;
		m_ModeArrow = nullptr;
		m_ModeLabelRoot = nullptr;
		m_ScoreRoot = nullptr;
	}

	void TitleSceneState::BindInput()
	{
		auto& input = InputManager::GetInstance();
		input.BindKeyboardInput(KeyCode::Up, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Up));
		input.BindKeyboardInput(KeyCode::Down, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Down));
		input.BindKeyboardInput(KeyCode::Return, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Confirm));

		const unsigned int controllerIndex = 0;
		input.AddController(controllerIndex);
		input.BindControllerInput(controllerIndex, ControllerButton::kDpadUp, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Up));
		input.BindControllerInput(controllerIndex, ControllerButton::kDpadDown, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Down));
		input.BindControllerInput(controllerIndex, ControllerButton::kX, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Confirm));
	}

	void TitleSceneState::UnbindInput()
	{
		auto& input = InputManager::GetInstance();
		input.UnBindKeyboardInput(KeyCode::Up, KeyState::Down);
		input.UnBindKeyboardInput(KeyCode::Down, KeyState::Down);
		input.UnBindKeyboardInput(KeyCode::Return, KeyState::Down);

		const unsigned int controllerIndex = 0;
		input.UnBindControllerInput(controllerIndex, ControllerButton::kDpadUp, KeyState::Down);
		input.UnBindControllerInput(controllerIndex, ControllerButton::kDpadDown, KeyState::Down);
		input.UnBindControllerInput(controllerIndex, ControllerButton::kX, KeyState::Down);
	}

	void TitleSceneState::MoveSelection(int direction)
	{
		const int newIndex = (m_SelectedIndex + direction + kMenuOptions) % kMenuOptions;
		if (newIndex != m_SelectedIndex)
		{
			m_SelectedIndex = newIndex;
			UpdateMenuSprites();
		}
	}

	void TitleSceneState::ConfirmSelection()
	{
		if (m_SelectedIndex == 0)
		{
			m_Owner.GetStateMachine().SetState(
				std::make_unique<TransitionSceneState>(
					m_Owner,
					"STAGE 1",
					std::make_unique<GameSceneState>(m_Owner, GetGameMode())
				)
			);
		}
		else if (m_SelectedIndex == 1)
		{
			CycleMode();
		}
	}

	void TitleSceneState::CycleMode()
	{
		if (m_SelectedIndex != 1)
			return;

		m_ModeIndex = (m_ModeIndex + 1) % kModeCount;
		switch (m_ModeIndex)
		{
		case 0:
			SetGameMode(GameMode::Solo);
			break;
		case 1:
			SetGameMode(GameMode::Coop);
			break;
		case 2:
			SetGameMode(GameMode::Versus);
			break;
		}
		UpdateModeLabel();
	}

	std::pair<float, float> TitleSceneState::GetCharacterSrcRect(char character) const
	{
		if (character >= '0' && character <= '9')
		{
			const int index = character - '0';
			return { index * kGlyphWidth, kGlyphRowY };
		}

		char upper = character;
		if (upper >= 'a' && upper <= 'z')
		{
			upper = static_cast<char>(upper - ('a' - 'A'));
		}
		if (upper >= 'A' && upper <= 'Z')
		{
			const int index = upper - 'A';
			return { index * kGlyphWidth, kGlyphRow2Y };
		}

		return { 0.0f, kGlyphRow2Y };
	}
}
