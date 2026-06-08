#pragma once
#include "BaseComponent.h"
#include <SDL3/SDL_rect.h>
#include <vector>
#include <string>

namespace dae
{
	class DeathAnimatorComponent final : public BaseComponent
	{
	public:
		DeathAnimatorComponent(GameObject* pOwner, const std::string& texture, const std::vector<SDL_FRect>& frames, float fps, float scale, bool broadcastEntityDied = true);

		void Update() override {}
		void Play();
		bool IsPlaying() const { return m_IsPlaying; }

	private:
		std::string m_DeathTexture;
		std::vector<SDL_FRect> m_DeathFrames;
		float m_DeathFps;
		float m_DeathScale;
		bool m_BroadcastEntityDied{ true };
		bool m_IsPlaying{ false };
	};
}
