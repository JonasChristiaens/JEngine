#include "TitleSceneState.h"
#include "GlyphHelper.h"
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
#include "Config/GameMode.h"
#include "Managers/HighScoreManager.h"
#include "Commands/ToggleMuteCommand.h"
#include "Audio/ServiceLocator.h"
#include "EventQueue/EventManager.h"
#include "Core/GameTime.h"
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

		Event playBgmEvent(make_sdbm_hash("PlayAudioEvent"));
		playBgmEvent.nbArgs = 1;
		playBgmEvent.args[0].p = "Title_Screen.flac";
		EventManager::GetInstance().BroadcastImmediate(playBgmEvent, m_pRoot);
		m_BgmCooldown = 1.0f;
	}

	void TitleSceneState::OnExit()
	{
		ServiceLocator::GetSoundService().StopAll();
		UnbindInput();
		ClearScene();
	}

	void TitleSceneState::Update()
	{
		const float deltaTime = GameTime::GetInstance().GetDeltaTime();

		m_BlinkTimer += deltaTime;
		if (m_BlinkTimer >= 0.25f)
		{
			m_BlinkTimer = 0.0f;
			m_ArrowVisible = !m_ArrowVisible;
			UpdateMenuSprites();
		}

		if (m_BgmCooldown > 0.0f)
		{
			m_BgmCooldown -= deltaTime;
			return;
		}

		if (!ServiceLocator::GetSoundService().IsPlaying())
		{
			Event playBgmEvent(make_sdbm_hash("PlayAudioEvent"));
			playBgmEvent.nbArgs = 1;
		playBgmEvent.args[0].p = "Title_Screen.flac";
			EventManager::GetInstance().BroadcastImmediate(playBgmEvent, m_pRoot);
			m_BgmCooldown = 1.0f;
		}
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
		m_pRoot = rootObject.get();
		auto rootTransform = m_pRoot->AddComponent<TransformComponent>();
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
		background->SetParent(m_pRoot, false);
		scene->Add(std::move(background));

		auto menuRoot = std::make_unique<GameObject>();
		menuRoot->AddComponent<TransformComponent>();
		menuRoot->SetParent(m_pRoot, false);
		m_pMenuRoot = menuRoot.get();
		scene->Add(std::move(menuRoot));

		auto startArrow = std::make_unique<GameObject>();
		auto startTransform = startArrow->AddComponent<TransformComponent>();
		startTransform->SetLocalPosition(kStartArrowX * m_ScaleX, kStartArrowY * m_ScaleY);
		m_pStartArrow = startArrow->AddComponent<RenderComponent>();
		m_pStartArrow->SetTexture("BombermanSprites_TitleScreen.png");
		m_pStartArrow->SetSourceRectangle(kArrowGlyphX, kGlyphRowY, kGlyphWidth, kGlyphHeight);
		m_pStartArrow->SetDestinationSize(m_ArrowWidth, m_ArrowHeight);
		m_pStartArrow->SetRenderLayer(kMenuLayer);
		startArrow->SetParent(m_pMenuRoot, false);
		scene->Add(std::move(startArrow));

		auto modeArrow = std::make_unique<GameObject>();
		auto modeTransform = modeArrow->AddComponent<TransformComponent>();
		modeTransform->SetLocalPosition(kModeArrowX * m_ScaleX, kModeArrowY * m_ScaleY);
		m_pModeArrow = modeArrow->AddComponent<RenderComponent>();
		m_pModeArrow->SetTexture("BombermanSprites_TitleScreen.png");
		m_pModeArrow->SetSourceRectangle(kArrowGlyphX, kGlyphRowY, kGlyphWidth, kGlyphHeight);
		m_pModeArrow->SetDestinationSize(m_ArrowWidth, m_ArrowHeight);
		m_pModeArrow->SetRenderLayer(kMenuLayer);
		modeArrow->SetParent(m_pMenuRoot, false);
		scene->Add(std::move(modeArrow));

		auto modeLabelRoot = std::make_unique<GameObject>();
		auto modeLabelTransform = modeLabelRoot->AddComponent<TransformComponent>();
		modeLabelTransform->SetLocalPosition(kModeLabelX * m_ScaleX, kModeLabelY * m_ScaleY);
		modeLabelRoot->SetParent(m_pMenuRoot, false);
		m_pModeLabelRoot = modeLabelRoot.get();
		scene->Add(std::move(modeLabelRoot));

		auto scoreRoot = std::make_unique<GameObject>();
		auto scoreTransform = scoreRoot->AddComponent<TransformComponent>();
		scoreTransform->SetLocalPosition(kScoreX * m_ScaleX, kScoreY * m_ScaleY);
		scoreRoot->SetParent(m_pMenuRoot, false);
		m_pScoreRoot = scoreRoot.get();
		scene->Add(std::move(scoreRoot));

		UpdateModeLabel();

		const int topScore = HighScoreManager::GetEntries()[0].score;
		std::ostringstream scoreStream;
		scoreStream << std::setfill('0') << std::setw(5) << topScore;
		CreateTextSprites(*scene, *m_pScoreRoot, scoreStream.str(), (kScoreWidth - (5.0f * kGlyphWidth)) * m_ScaleX, 0.0f, kMenuLayer);
	}

	void TitleSceneState::UpdateMenuSprites()
	{
		if (m_pStartArrow)
		{
			const bool visible = (m_SelectedIndex == 0) && m_ArrowVisible;
			m_pStartArrow->SetDestinationSize(visible ? m_ArrowWidth : 0.0f, m_ArrowHeight);
		}
		if (m_pModeArrow)
		{
			const bool visible = (m_SelectedIndex == 1) && m_ArrowVisible;
			m_pModeArrow->SetDestinationSize(visible ? m_ArrowWidth : 0.0f, m_ArrowHeight);
		}
	}

	void TitleSceneState::UpdateModeLabel()
	{
		if (!m_pModeLabelRoot)
			return;

		m_pModeLabelRoot->MarkForDeletion();
		m_pModeLabelRoot = nullptr;

		auto* scene = m_Owner.GetActiveScene();
		if (!scene)
			return;
		auto labelRoot = std::make_unique<GameObject>();
		auto labelTransform = labelRoot->AddComponent<TransformComponent>();
		labelTransform->SetLocalPosition(kModeLabelX * m_ScaleX, kModeLabelY * m_ScaleY);
		labelRoot->SetParent(m_pMenuRoot, false);
		m_pModeLabelRoot = labelRoot.get();
		scene->Add(std::move(labelRoot));

		CreateTextSprites(*scene, *m_pModeLabelRoot, kModeLabels[m_ModeIndex], 0.0f, 0.0f, kMenuLayer);
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

		m_pRoot = nullptr;
		m_pMenuRoot = nullptr;
		m_pStartArrow = nullptr;
		m_pModeArrow = nullptr;
		m_pModeLabelRoot = nullptr;
		m_pScoreRoot = nullptr;
	}

	void TitleSceneState::BindInput()
	{
		auto& input = InputManager::GetInstance();
		input.BindKeyboardInput(KeyCode::Left, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Up));
		input.BindKeyboardInput(KeyCode::Right, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Down));
		input.BindKeyboardInput(KeyCode::Return, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Confirm));

		const unsigned int controllerIndex = 0;
		input.AddController(controllerIndex);
		input.BindControllerInput(controllerIndex, ControllerButton::kDpadLeft, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Up));
		input.BindControllerInput(controllerIndex, ControllerButton::kDpadRight, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Down));
		input.BindControllerInput(controllerIndex, ControllerButton::kX, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Confirm));

		auto mute = std::make_unique<ToggleMuteCommand>();
		mute->SetGameActor(m_pRoot);
		input.BindKeyboardInput(KeyCode::F2, KeyState::Down, std::move(mute));
	}

	void TitleSceneState::UnbindInput()
	{
		auto& input = InputManager::GetInstance();
		input.UnBindKeyboardInput(KeyCode::F2, KeyState::Down);
		input.UnBindKeyboardInput(KeyCode::Left, KeyState::Down);
		input.UnBindKeyboardInput(KeyCode::Right, KeyState::Down);
		input.UnBindKeyboardInput(KeyCode::Return, KeyState::Down);

		const unsigned int controllerIndex = 0;
		input.UnBindControllerInput(controllerIndex, ControllerButton::kDpadLeft, KeyState::Down);
		input.UnBindControllerInput(controllerIndex, ControllerButton::kDpadRight, KeyState::Down);
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
		return GlyphHelper::GetCharacterSrcRect(character);
	}
}
