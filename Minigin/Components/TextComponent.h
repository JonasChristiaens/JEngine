#pragma once
#include "BaseComponent.h"
#include <string>
#include <memory>
#include <SDL3/SDL.h>

namespace dae
{
	class Font;
	class Texture2D;
	class RenderComponent;

	class TextComponent final : public BaseComponent
	{
	public:
		TextComponent(GameObject* pOwner, const std::string& text, std::shared_ptr<Font> font);
		virtual ~TextComponent() = default;

		void Update() override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

	private:
		bool m_NeedsUpdate{};
		std::string m_Text{};
		SDL_Color m_Color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_Font{};
		RenderComponent* m_pRenderComponent{};
	};
}