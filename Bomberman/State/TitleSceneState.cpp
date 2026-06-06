#include "TitleSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Input/InputManager.h"
#include "Input/ControllerButtons.h"
#include "Rendering/Renderer.h"
#include "State/GameSceneState.h"
#include "Commands/Command.h"
#include <utility>

namespace
{
	constexpr float kTitleSpriteWidth = 256.0f;
	constexpr float kTitleSpriteHeight = 240.0f;
	constexpr float kStartArrowX = 65.0f;
	constexpr float kStartArrowY = 153.0f;
	constexpr float kModeArrowX = 129.0f;
	constexpr float kModeArrowY = 153.0f;
	constexpr float kModeLabelX = 177.0f;
	constexpr float kModeLabelY = 153.0f;
	constexpr float kScoreX = 113.0f;
	constexpr float kScoreY = 169.0f;
	constexpr float kGlyphWidth = 8.0f;
	constexpr float kGlyphHeight = 8.0f;
	constexpr float kGlyphRowY = 249.0f;
	constexpr float kGlyphRow2Y = 257.0f;
	constexpr float kArrowGlyphX = 81.0f;
	constexpr float kScoreWidth = 72.0f;
	constexpr float kModeLabelWidth = 32.0f;
	constexpr int kMenuLayer = 2;
	constexpr int kBackgroundLayer = 0;
	constexpr int kMenuOptions = 2;

	const char* kModeLabels[] = { "SOLO", "COOP", "1VS1" };
	constexpr int kModeCount = 3;
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
				: m_state(state)
				, m_action(action)
			{
			}

			void Execute() override
			{
				switch (m_action)
				{
				case Action::Up:
					m_state.MoveSelection(-1);
					break;
				case Action::Down:
					m_state.MoveSelection(1);
					break;
				case Action::Confirm:
					m_state.ConfirmSelection();
					break;
				case Action::CycleMode:
					m_state.CycleMode();
					break;
				}
			}

		private:
			TitleSceneState& m_state;
			Action m_action;
		};
	}

	TitleSceneState::TitleSceneState(SceneStateMachineComponent& owner)
		: SceneState(owner)
	{
	}

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
		auto* scene = m_owner.GetActiveScene();
		if (!scene)
		{
			auto& newScene = SceneManager::GetInstance().CreateScene();
			m_owner.SetActiveScene(newScene);
			scene = m_owner.GetActiveScene();
		}
		if (!scene)
			return;

		const auto windowSize = Renderer::GetInstance().GetWindowSize();
		m_scaleY = static_cast<float>(windowSize.y) / kTitleSpriteHeight;
		m_scaleX = static_cast<float>(windowSize.x) / kTitleSpriteWidth;
		const float scale = m_scaleY;
		const float scaledWidth = kTitleSpriteWidth * scale;
		const float scaledHeight = kTitleSpriteHeight * scale;
		const float offsetX = (static_cast<float>(windowSize.x) - scaledWidth) * 0.5f;
		const float offsetY = (static_cast<float>(windowSize.y) - scaledHeight) * 0.5f;
		m_scaleX = scale;
		m_arrowWidth = kGlyphWidth * m_scaleX;
		m_arrowHeight = kGlyphHeight * m_scaleY;

		auto rootObject = std::make_unique<GameObject>();
		m_root = rootObject.get();
		auto rootTransform = m_root->AddComponent<TransformComponent>();
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
		background->SetParent(m_root, false);
		scene->Add(std::move(background));

		auto menuRoot = std::make_unique<GameObject>();
		menuRoot->AddComponent<TransformComponent>();
		menuRoot->SetParent(m_root, false);
		m_menuRoot = menuRoot.get();
		scene->Add(std::move(menuRoot));

		auto startArrow = std::make_unique<GameObject>();
		auto startTransform = startArrow->AddComponent<TransformComponent>();
		startTransform->SetLocalPosition(kStartArrowX * m_scaleX, kStartArrowY * m_scaleY);
		m_startArrow = startArrow->AddComponent<RenderComponent>();
		m_startArrow->SetTexture("BombermanSprites_TitleScreen.png");
		m_startArrow->SetSourceRectangle(kArrowGlyphX, kGlyphRowY, kGlyphWidth, kGlyphHeight);
		m_startArrow->SetDestinationSize(m_arrowWidth, m_arrowHeight);
		m_startArrow->SetRenderLayer(kMenuLayer);
		startArrow->SetParent(m_menuRoot, false);
		scene->Add(std::move(startArrow));

		auto modeArrow = std::make_unique<GameObject>();
		auto modeTransform = modeArrow->AddComponent<TransformComponent>();
		modeTransform->SetLocalPosition(kModeArrowX * m_scaleX, kModeArrowY * m_scaleY);
		m_modeArrow = modeArrow->AddComponent<RenderComponent>();
		m_modeArrow->SetTexture("BombermanSprites_TitleScreen.png");
		m_modeArrow->SetSourceRectangle(kArrowGlyphX, kGlyphRowY, kGlyphWidth, kGlyphHeight);
		m_modeArrow->SetDestinationSize(m_arrowWidth, m_arrowHeight);
		m_modeArrow->SetRenderLayer(kMenuLayer);
		modeArrow->SetParent(m_menuRoot, false);
		scene->Add(std::move(modeArrow));

		auto modeLabelRoot = std::make_unique<GameObject>();
		auto modeLabelTransform = modeLabelRoot->AddComponent<TransformComponent>();
		modeLabelTransform->SetLocalPosition(kModeLabelX * m_scaleX, kModeLabelY * m_scaleY);
		modeLabelRoot->SetParent(m_menuRoot, false);
		m_modeLabelRoot = modeLabelRoot.get();
		scene->Add(std::move(modeLabelRoot));

		auto scoreRoot = std::make_unique<GameObject>();
		auto scoreTransform = scoreRoot->AddComponent<TransformComponent>();
		scoreTransform->SetLocalPosition(kScoreX * m_scaleX, kScoreY * m_scaleY);
		scoreRoot->SetParent(m_menuRoot, false);
		m_scoreRoot = scoreRoot.get();
		scene->Add(std::move(scoreRoot));

		UpdateModeLabel();
		CreateTextSprites(*scene, *m_scoreRoot, "00", (kScoreWidth - (2.0f * kGlyphWidth)) * m_scaleX, 0.0f, kMenuLayer);
	}

	void TitleSceneState::UpdateMenuSprites()
	{
		if (m_startArrow)
		{
			const float width = (m_selectedIndex == 0) ? m_arrowWidth : 0.0f;
			m_startArrow->SetDestinationSize(width, m_arrowHeight);
		}
		if (m_modeArrow)
		{
			const float width = (m_selectedIndex == 1) ? m_arrowWidth : 0.0f;
			m_modeArrow->SetDestinationSize(width, m_arrowHeight);
		}
	}

	void TitleSceneState::UpdateModeLabel()
	{
		if (!m_modeLabelRoot)
			return;

		m_modeLabelRoot->MarkForDeletion();
		m_modeLabelRoot = nullptr;

		auto* scene = m_owner.GetActiveScene();
		if (!scene)
			return;
		auto labelRoot = std::make_unique<GameObject>();
		auto labelTransform = labelRoot->AddComponent<TransformComponent>();
		labelTransform->SetLocalPosition(kModeLabelX * m_scaleX, kModeLabelY * m_scaleY);
		labelRoot->SetParent(m_menuRoot, false);
		m_modeLabelRoot = labelRoot.get();
		scene->Add(std::move(labelRoot));

		CreateTextSprites(*scene, *m_modeLabelRoot, kModeLabels[m_modeIndex], 0.0f, 0.0f, kMenuLayer);
	}

	void TitleSceneState::CreateTextSprites(Scene& scene, GameObject& parent, const std::string& text, float x, float y, int renderLayer)
	{
		float cursorX = x;

		for (char c : text)
		{
			if (c == ' ')
			{
				cursorX += kGlyphWidth * m_scaleX;
				continue;
			}
			auto [glyphX, glyphY] = GetCharacterSrcRect(c);

			auto glyph = std::make_unique<GameObject>();
			auto glyphTransform = glyph->AddComponent<TransformComponent>();
			glyphTransform->SetLocalPosition(cursorX, y);
			auto glyphRender = glyph->AddComponent<RenderComponent>();
			glyphRender->SetTexture("BombermanSprites_TitleScreen.png");
			glyphRender->SetSourceRectangle(glyphX, glyphY, kGlyphWidth, kGlyphHeight);
			glyphRender->SetDestinationSize(kGlyphWidth * m_scaleX, kGlyphHeight * m_scaleY);
			glyphRender->SetRenderLayer(renderLayer);
			glyph->SetParent(&parent, false);
			scene.Add(std::move(glyph));

			cursorX += kGlyphWidth * m_scaleX;
		}
	}

	void TitleSceneState::ClearScene()
	{
		if (auto* scene = m_owner.GetActiveScene())
		{
			scene->RemoveAll();
		}

		m_root = nullptr;
		m_menuRoot = nullptr;
		m_startArrow = nullptr;
		m_modeArrow = nullptr;
		m_modeLabelRoot = nullptr;
		m_scoreRoot = nullptr;
	}

	void TitleSceneState::BindInput()
	{
		auto& input = InputManager::GetInstance();
		input.BindKeyboardInput(SDLK_UP, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Up));
		input.BindKeyboardInput(SDLK_DOWN, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Down));
        input.BindKeyboardInput(SDLK_RETURN, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Confirm));
		input.BindKeyboardInput(SDLK_LEFT, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::CycleMode));
		input.BindKeyboardInput(SDLK_RIGHT, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::CycleMode));

		const unsigned int controllerIndex = 0;
		input.AddController(controllerIndex);
		input.BindControllerInput(controllerIndex, ControllerButton::DPadUp, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Up));
		input.BindControllerInput(controllerIndex, ControllerButton::DPadDown, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Down));
		input.BindControllerInput(controllerIndex, ControllerButton::DPadLeft, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::CycleMode));
		input.BindControllerInput(controllerIndex, ControllerButton::DPadRight, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::CycleMode));
		input.BindControllerInput(controllerIndex, ControllerButton::X, KeyState::Down, std::make_unique<TitleMenuCommand>(*this, TitleMenuCommand::Action::Confirm));
	}

	void TitleSceneState::UnbindInput()
	{
		auto& input = InputManager::GetInstance();
		input.UnBindKeyboardInput(SDLK_UP, KeyState::Down);
		input.UnBindKeyboardInput(SDLK_DOWN, KeyState::Down);
		input.UnBindKeyboardInput(SDLK_LEFT, KeyState::Down);
		input.UnBindKeyboardInput(SDLK_RIGHT, KeyState::Down);
        input.UnBindKeyboardInput(SDLK_RETURN, KeyState::Down);

		const unsigned int controllerIndex = 0;
		input.UnBindControllerInput(controllerIndex, ControllerButton::DPadUp, KeyState::Down);
		input.UnBindControllerInput(controllerIndex, ControllerButton::DPadDown, KeyState::Down);
		input.UnBindControllerInput(controllerIndex, ControllerButton::DPadLeft, KeyState::Down);
		input.UnBindControllerInput(controllerIndex, ControllerButton::DPadRight, KeyState::Down);
		input.UnBindControllerInput(controllerIndex, ControllerButton::X, KeyState::Down);
	}

	void TitleSceneState::MoveSelection(int direction)
	{
		const int newIndex = (m_selectedIndex + direction + kMenuOptions) % kMenuOptions;
		if (newIndex != m_selectedIndex)
		{
			m_selectedIndex = newIndex;
			UpdateMenuSprites();
		}
	}

	void TitleSceneState::ConfirmSelection()
	{
		if (m_selectedIndex == 0)
		{
			m_owner.GetStateMachine().SetState(std::make_unique<GameSceneState>(m_owner));
		}
		else if (m_selectedIndex == 1)
		{
			CycleMode();
		}
	}

	void TitleSceneState::CycleMode()
	{
		if (m_selectedIndex != 1)
			return;

		m_modeIndex = (m_modeIndex + 1) % kModeCount;
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
