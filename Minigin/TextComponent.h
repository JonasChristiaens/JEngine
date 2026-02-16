#pragma once
#include "BaseComponent.h"
#include <string>
#include <memory>
#include <SDL3/SDL.h>

namespace dae
{
	class Font;
	class Texture2D;
	class TransformComponent;

	class TextComponent final : public BaseComponent
	{
	public:
		TextComponent(GameObject* pOwner, const std::string& text, std::shared_ptr<Font> font);
		virtual ~TextComponent() = default;

		void Update(float /*deltaTime*/) override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

	private:
		bool m_needsUpdate{};
		std::string m_text{};
		SDL_Color m_color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_font{};
		std::shared_ptr<Texture2D> m_textTexture{};
	};
}