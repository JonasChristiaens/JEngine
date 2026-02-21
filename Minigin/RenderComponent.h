#pragma once
#include "BaseComponent.h"
#include <memory>
#include <string>
#include <SDL3/SDL.h>

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
		void SetTexture(std::shared_ptr<Texture2D> texture);	

		void SetSourceRectangle(float x, float y, float width, float height);
		void SetSourceRectangle(const SDL_FRect& rect);
		void ClearSourceRectangle();

		void SetSpriteSheet(int spriteWidth, int spriteHeight, int columns, int rows);
		void SetSprite(int column, int row);

		void SetScale(float scale);
		float GetScale() const { return m_scale; }

	private:
		std::shared_ptr<Texture2D> m_texture{};
		SDL_FRect m_sourceRect{};
		bool m_useSourceRect{ false };

		int m_spriteWidth{ 0 };
		int m_spriteHeight{ 0 };
		int m_columns{ 0 };
		int m_rows{ 0 };

		float m_scale{ 1.0f };
	};
}