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

	const auto& pos = transform->GetPosition();
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
	m_texture = std::move(texture);
}