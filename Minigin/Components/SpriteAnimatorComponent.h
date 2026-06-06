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

		bool IsFinished() const { return m_IsFinished; }
		bool IsPlaying() const { return m_IsPlaying; }

		template <typename Callback>
		void SetOnAnimationFinishedCallback(Callback&& callback)
		{
			m_OnFinishedCallback = std::forward<Callback>(callback);
		}

	private:
		void UpdateSprite();

		RenderComponent* m_pRenderComponent{ nullptr };

		int m_StartColumn{ 0 };
		int m_StartRow{ 0 };
		int m_Columns{ 1 };
		int m_FrameCount{ 1 };

		std::vector<SDL_FRect> m_ExplicitFrames{};
		bool m_UseExplicitFrames{ false };

		int m_CurrentFrame{ 0 };
		float m_AnimationTimer{ 0.0f };
		float m_FrameDuration{ 0.1f };

		bool m_IsLooping{ true };
		bool m_IsPlaying{ false };
		bool m_IsFinished{ false };

		std::function<void()> m_OnFinishedCallback{ nullptr };
	};
}
