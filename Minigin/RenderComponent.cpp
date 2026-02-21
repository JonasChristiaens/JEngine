#include "RenderComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"

dae::RenderComponent::RenderComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::RenderComponent::Render() const
{
	if (m_texture == nullptr)
		return;

	auto transform = GetOwner()->GetComponent<TransformComponent>();
	if (transform == nullptr)
		return;

	const auto& pos = transform->GetLocalPosition();

	if (m_useSourceRect)
	{
		const float scaledWidth = m_sourceRect.w * m_scale;
		const float scaledHeight = m_sourceRect.h * m_scale;
		Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y, scaledWidth, scaledHeight, m_sourceRect);
	}
	else
	{
		Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
	}
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
	m_texture = std::move(texture);
}

void dae::RenderComponent::SetSourceRectangle(float x, float y, float width, float height)
{
	m_sourceRect = SDL_FRect{ x, y, width, height };
	m_useSourceRect = true;
}

void dae::RenderComponent::SetSourceRectangle(const SDL_FRect& rect)
{
	m_sourceRect = rect;
	m_useSourceRect = true;
}

void dae::RenderComponent::ClearSourceRectangle()
{
	m_useSourceRect = false;
}

void dae::RenderComponent::SetScale(float scale)
{
	m_scale = scale;
}

void dae::RenderComponent::SetSpriteSheet(int spriteWidth, int spriteHeight, int columns, int rows)
{
	m_spriteWidth = spriteWidth;
	m_spriteHeight = spriteHeight;
	m_columns = columns;
	m_rows = rows;
}

void dae::RenderComponent::SetSprite(int column, int row)
{
	if (m_spriteWidth == 0 || m_spriteHeight == 0)
		return;

	const float x = static_cast<float>(column * m_spriteWidth);
	const float y = static_cast<float>(row * m_spriteHeight);
	SetSourceRectangle(x, y, static_cast<float>(m_spriteWidth), static_cast<float>(m_spriteHeight));
}