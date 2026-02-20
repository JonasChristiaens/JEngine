#include "TextComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "Font.h"
#include "Texture2D.h"
#include "Renderer.h"
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>

dae::TextComponent::TextComponent(GameObject* pOwner, const std::string& text, std::shared_ptr<Font> font)
	: BaseComponent(pOwner)
	, m_needsUpdate(true)
	, m_text(text)
	, m_font(std::move(font))
{
	m_pRenderComponent = GetOwner()->GetComponent<RenderComponent>();
	if (m_pRenderComponent == nullptr)
	{
		m_pRenderComponent = GetOwner()->AddComponent<RenderComponent>();
	}
}

void dae::TextComponent::Update(float /*deltaTime*/)
{
	if (m_needsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), m_text.length(), m_color);
		if (surf == nullptr)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr)
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_DestroySurface(surf);
		auto textTexture = std::make_shared<Texture2D>(texture);
		m_pRenderComponent->SetTexture(textTexture);
		m_needsUpdate = false;
	}
}

void dae::TextComponent::Render() const
{
	if (m_pRenderComponent != nullptr)
	{
		m_pRenderComponent->Render();
	}
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_text = text;
	m_needsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_color = color;
	m_needsUpdate = true;
}