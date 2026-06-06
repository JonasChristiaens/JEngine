#include <algorithm>
#include "Scene.h"
#include <cassert>
#include "Components/RenderComponent.h"

namespace dae
{

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_Objects.emplace_back(std::move(object));
}

void Scene::RemoveAll()
{
	m_Objects.clear();
}

bool Scene::Contains(const GameObject* object) const
{
	if (!object)
		return false;

	return std::any_of(m_Objects.begin(), m_Objects.end(), [object](const std::unique_ptr<GameObject>& entry)
		{
			return entry.get() == object;
		});
}

void Scene::Update()
{
	for (auto& object : m_Objects)
	{
		if (object->GetParent() == nullptr)
		{
			object->Update();
		}
	}
}

void Scene::LateUpdate()
{
	m_Objects.erase(
		std::remove_if(
			m_Objects.begin(),
			m_Objects.end(),
			[](const auto& object) { return object->IsMarkedForDeletion(); }
		),
		m_Objects.end()
	);
}

void Scene::Render() const
{
	std::vector<GameObject*> sortedObjects{};
	sortedObjects.reserve(m_Objects.size());

	for (const auto& object : m_Objects)
	{
		if (object->GetParent() == nullptr)
		{
			sortedObjects.push_back(object.get());
		}
	}

	std::stable_sort(sortedObjects.begin(), sortedObjects.end(), [](const GameObject* lhs, const GameObject* rhs)
	{
		const auto* lhsRender = lhs->GetComponent<RenderComponent>();
		const auto* rhsRender = rhs->GetComponent<RenderComponent>();

		const int lhsLayer = lhsRender ? lhsRender->GetRenderLayer() : 0;
		const int rhsLayer = rhsRender ? rhsRender->GetRenderLayer() : 0;

		return lhsLayer < rhsLayer;
	});

	for (const auto* object : sortedObjects)
	{
		object->Render();
	}
}

}
