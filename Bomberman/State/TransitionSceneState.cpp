#include "TransitionSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "Components/DelayedEventComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "EventQueue/EventManager.h"
#include "Rendering/Renderer.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "State/StateMachine.h"
#include <SDL3/SDL.h>

namespace
{
	constexpr float kGlyphWidth{ 8.0f };
	constexpr float kGlyphHeight{ 8.0f };
	constexpr float kGlyphRowY{ 264.0f };
	constexpr float kGlyphRow2Y{ 272.0f };
	constexpr float kTextScale{ 4.0f };
	constexpr int kTextRenderLayer{ 1 };
	constexpr float kTransitionDelay{ 2.0f };
	constexpr dae::EventId kTransitionCompleteEventId = dae::make_sdbm_hash("TransitionComplete");
}

namespace dae
{
	TransitionSceneState::TransitionSceneState(SceneStateMachineComponent& owner,
		const std::string& text,
		std::unique_ptr<State> nextState)
		: SceneState(owner)
		, m_Text(text)
		, m_NextState(std::move(nextState))
	{
	}

	TransitionSceneState::~TransitionSceneState()
	{
		if (EventManager::IsAlive())
			EventManager::GetInstance().RemoveObserver(*this);
	}

	void TransitionSceneState::OnEnter()
	{
		EventManager::GetInstance().AddObserver(*this);
		BuildScene();
	}

	void TransitionSceneState::OnExit()
	{
		if (EventManager::IsAlive())
			EventManager::GetInstance().RemoveObserver(*this);

		ClearScene();
	}

	void TransitionSceneState::Update()
	{
		if (m_ShouldTransition && m_NextState)
		{
			m_ShouldTransition = false;
			m_Owner.GetStateMachine().SetState(std::move(m_NextState));
		}
	}

	void TransitionSceneState::Render() const
	{
		SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();

		Uint8 prevR, prevG, prevB, prevA;
		SDL_GetRenderDrawColor(renderer, &prevR, &prevG, &prevB, &prevA);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		const SDL_Point windowSize = Renderer::GetInstance().GetWindowSize();
		const SDL_FRect fullScreen{ 0.0f, 0.0f, static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) };
		SDL_RenderFillRect(renderer, &fullScreen);

		SDL_SetRenderDrawColor(renderer, prevR, prevG, prevB, prevA);
	}

	void TransitionSceneState::Notify(GameObject& /*actor*/, Event event)
	{
		if (event.id == kTransitionCompleteEventId)
		{
			m_ShouldTransition = true;
		}
	}

	void TransitionSceneState::BuildScene()
	{
		auto& scene = SceneManager::GetInstance().CreateScene();
		m_pScene = &scene;
		m_Owner.SetActiveScene(scene);

		const SDL_Point windowSize = Renderer::GetInstance().GetWindowSize();
		const float windowWidth = static_cast<float>(windowSize.x);
		const float windowHeight = static_cast<float>(windowSize.y);
		const float glyphSize = kGlyphWidth * kTextScale;

		size_t visibleCount{ 0 };
		for (char c : m_Text)
		{
			if (c != ' ')
				++visibleCount;
		}
		const float textWidth = static_cast<float>(visibleCount) * glyphSize;
		const float startX = (windowWidth - textWidth) * 0.5f;
		const float startY = (windowHeight - glyphSize) * 0.5f;

		auto rootObject = std::make_unique<GameObject>();
		m_Root = rootObject.get();
		rootObject->AddComponent<TransformComponent>();
		scene.Add(std::move(rootObject));

		CreateTextSprites(scene, *m_Root, m_Text, startX, startY);

		const Event transitionEvent(kTransitionCompleteEventId);
		auto timerObject = std::make_unique<GameObject>();
		timerObject->AddComponent<DelayedEventComponent>(transitionEvent, kTransitionDelay);
		timerObject->SetParent(m_Root, false);
		scene.Add(std::move(timerObject));
	}

	void TransitionSceneState::CreateTextSprites(Scene& scene, GameObject& parent, const std::string& text, float startX, float startY)
	{
		float cursorX = startX;
		const float glyphSize = kGlyphWidth * kTextScale;

		for (char c : text)
		{
			if (c == ' ')
			{
				cursorX += glyphSize;
				continue;
			}

			auto [glyphX, glyphY] = GetCharacterSrcRect(c);

			auto glyph = std::make_unique<GameObject>();
			auto glyphTransform = glyph->AddComponent<TransformComponent>();
			glyphTransform->SetLocalPosition(cursorX, startY);
			auto glyphRender = glyph->AddComponent<RenderComponent>();
			glyphRender->SetTexture("BombermanSprites_TitleScreen.png");
			glyphRender->SetSourceRectangle(glyphX, glyphY, kGlyphWidth, kGlyphHeight);
			glyphRender->SetDestinationSize(glyphSize, glyphSize);
			glyphRender->SetRenderLayer(kTextRenderLayer);
			glyph->SetParent(&parent, false);
			scene.Add(std::move(glyph));

			cursorX += glyphSize;
		}
	}

	void TransitionSceneState::ClearScene()
	{
		m_Root = nullptr;

		if (m_pScene)
		{
			m_pScene->RemoveAll();
			m_pScene = nullptr;
		}
	}

	std::pair<float, float> TransitionSceneState::GetCharacterSrcRect(char character) const
	{
		if (character >= '0' && character <= '9')
		{
			const int index = character - '0';
			return { static_cast<float>(index) * kGlyphWidth, kGlyphRowY };
		}

		char upper = character;
		if (upper >= 'a' && upper <= 'z')
		{
			upper = static_cast<char>(upper - ('a' - 'A'));
		}
		if (upper >= 'A' && upper <= 'Z')
		{
			const int index = upper - 'A';
			return { static_cast<float>(index) * kGlyphWidth, kGlyphRow2Y };
		}

		return { 0.0f, kGlyphRow2Y };
	}
}
