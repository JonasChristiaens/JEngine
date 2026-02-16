#pragma once
#include "BaseComponent.h"
#include <memory>
#include <string>

namespace dae
{
	class Texture2D;
	class TransformComponent;

	class RenderComponent final : public BaseComponent
	{
	public:
		RenderComponent(GameObject* pOwner);
		virtual ~RenderComponent() = default;

		void Update(float /*deltaTime*/) override {}
		void Render() const override;

		void SetTexture(const std::string& filename);

	private:
		std::shared_ptr<Texture2D> m_texture{};
	};
}