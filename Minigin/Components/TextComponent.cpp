#include "TextComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "Font.h"
#include "Texture2D.h"
#include "Renderer.h"
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>

dae::TextComponent::TextComponent(GameObject* pOwner, const std::string& text, std::unique_ptr<Font> font)
	: BaseComponent(pOwner)
	, m_NeedsUpdate(true)
	, m_Text(text)
	, m_Font(std::move(font))
{
	m_pRenderComponent = GetOwner()->GetComponent<RenderComponent>();
	if (m_pRenderComponent == nullptr)
	{
		m_pRenderComponent = GetOwner()->AddComponent<RenderComponent>();
	}
}

dae::TextComponent::~TextComponent() = default;

void dae::TextComponent::Update()
{
	if (m_NeedsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
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
		m_pRenderComponent->SetTexture(std::make_unique<Texture2D>(texture));
		m_NeedsUpdate = false;
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
	m_Text = text;
	m_NeedsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
	m_NeedsUpdate = true;
}