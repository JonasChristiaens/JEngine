#pragma once
#include "BaseComponent.h"
#include <memory>
#include <string>
#include <SDL3/SDL.h>
#include <glm/vec2.hpp>

namespace dae
{
	class Texture2D;
	class TransformComponent;

	class RenderComponent final : public BaseComponent
	{
	public:
		explicit RenderComponent(GameObject* pOwner);
		~RenderComponent();

		void Update() override {}
		void Render() const override;

		void SetTexture(const std::string& filename);
		void SetTexture(std::unique_ptr<Texture2D> texture);

		void SetSourceRectangle(float x, float y, float width, float height);
		void SetSourceRectangle(const SDL_FRect& rect);
		void ClearSourceRectangle();
		void SetDestinationSize(float width, float height);
		void ClearDestinationSize();

		void SetSpriteSheet(int spriteWidth, int spriteHeight, int columns, int rows);
		void SetSprite(int column, int row);

		void SetScale(float scale);
		float GetScale() const noexcept { return m_Scale; }
		void SetPivot(const glm::vec2& pivot);

		void SetRenderLayer(int layer) noexcept { m_RenderLayer = layer; }
		int GetRenderLayer() const noexcept { return m_RenderLayer; }

	private:
		std::unique_ptr<Texture2D> m_Texture{};
		SDL_FRect m_SourceRect{};
		bool m_UseSourceRect{ false };

		int m_SpriteWidth{ 0 };
		int m_SpriteHeight{ 0 };
		int m_Columns{ 0 };
		int m_Rows{ 0 };

		float m_Scale{ 1.0f };
		float m_DestinationWidth{ 0.0f };
		float m_DestinationHeight{ 0.0f };
		int m_RenderLayer{ 0 };
		glm::vec2 m_Pivot{ 0.0f, 0.0f };
		bool m_UseDestinationSize{ false };

		mutable TransformComponent* m_pTransform{};
	};
}