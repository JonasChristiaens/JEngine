#include "RenderComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"

dae::RenderComponent::RenderComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{}

dae::RenderComponent::~RenderComponent() = default;

void dae::RenderComponent::Render() const
{
	if (m_Texture == nullptr)
		return;

	if (m_pTransform == nullptr)
		m_pTransform = GetOwner()->GetComponent<TransformComponent>();
	if (m_pTransform == nullptr)
		return;

	const auto& pos = m_pTransform->GetWorldPosition();

	if (m_UseSourceRect)
	{
		const float scaledWidth = m_UseDestinationSize ? m_DestinationWidth : m_SourceRect.w * m_Scale;
		const float scaledHeight = m_UseDestinationSize ? m_DestinationHeight : m_SourceRect.h * m_Scale;
		Renderer::GetInstance().RenderTexture(
			*m_Texture,
			pos.x - m_Pivot.x * scaledWidth,
			pos.y - m_Pivot.y * scaledHeight,
			scaledWidth,
			scaledHeight,
			m_SourceRect
		);
	}
	else
	{
		if (m_UseDestinationSize)
		{
			Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y, m_DestinationWidth, m_DestinationHeight);
		}
		else
		{
			Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
		}
	}
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::RenderComponent::SetTexture(std::unique_ptr<Texture2D> texture)
{
	m_Texture = std::move(texture);
}

void dae::RenderComponent::SetSourceRectangle(float x, float y, float width, float height)
{
	m_SourceRect = SDL_FRect{ x, y, width, height };
	m_UseSourceRect = true;
}

void dae::RenderComponent::SetSourceRectangle(const SDL_FRect& rect)
{
	m_SourceRect = rect;
	m_UseSourceRect = true;
}

void dae::RenderComponent::ClearSourceRectangle()
{
	m_UseSourceRect = false;
}

void dae::RenderComponent::SetScale(float scale)
{
	m_Scale = scale;
}

void dae::RenderComponent::SetDestinationSize(float width, float height)
{
	m_DestinationWidth = width;
	m_DestinationHeight = height;
	m_UseDestinationSize = true;
}

void dae::RenderComponent::ClearDestinationSize()
{
	m_UseDestinationSize = false;
}

void dae::RenderComponent::SetPivot(const glm::vec2& pivot)
{
	m_Pivot = pivot;
}

void dae::RenderComponent::SetSpriteSheet(int spriteWidth, int spriteHeight, int columns, int rows)
{
	m_SpriteWidth = spriteWidth;
	m_SpriteHeight = spriteHeight;
	m_Columns = columns;
	m_Rows = rows;
}

void dae::RenderComponent::SetSprite(int column, int row)
{
	if (m_SpriteWidth == 0 || m_SpriteHeight == 0)
		return;

	const float x = static_cast<float>(column * m_SpriteWidth);
	const float y = static_cast<float>(row * m_SpriteHeight);
	SetSourceRectangle(x, y, static_cast<float>(m_SpriteWidth), static_cast<float>(m_SpriteHeight));
}