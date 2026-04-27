#pragma once
#include "BaseComponent.h"
#include <vector>
#include <functional>
#include <SDL3/SDL.h>

namespace dae
{
	class RenderComponent;

	class SpriteAnimatorComponent final : public BaseComponent
	{
	public:
		SpriteAnimatorComponent(GameObject* pOwner);
		virtual ~SpriteAnimatorComponent() = default;

		void Update() override;
		void Render() const override {}

		// Existing grid animation
		void SetAnimation(int startColumn, int startRow, int columns, int frameCount, float framesPerSecond, bool isLooping);
		void SetAnimation(const std::vector<SDL_FRect>& frames, float framesPerSecond, bool isLooping);

		void Play();
		void Pause();
		void Stop();

		bool IsFinished() const { return m_isFinished; }
		bool IsPlaying() const { return m_isPlaying; }

		template <typename Callback>
		void SetOnAnimationFinishedCallback(Callback&& callback)
		{
			m_onFinishedCallback = std::forward<Callback>(callback);
		}

	private:
		void UpdateSprite();

		RenderComponent* m_pRenderComponent{ nullptr };

		int m_startColumn{ 0 };
		int m_startRow{ 0 };
		int m_columns{ 1 };
		int m_frameCount{ 1 };

		std::vector<SDL_FRect> m_explicitFrames{};
		bool m_useExplicitFrames{ false };

		int m_currentFrame{ 0 };
		float m_animationTimer{ 0.0f };
		float m_frameDuration{ 0.1f };

		bool m_isLooping{ true };
		bool m_isPlaying{ false };
		bool m_isFinished{ false };

		std::function<void()> m_onFinishedCallback{ nullptr };
	};
}
