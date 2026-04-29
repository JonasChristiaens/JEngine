#include <algorithm>
#include "Scene.h"
#include <cassert>
#include "Components/RenderComponent.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_objects.emplace_back(std::move(object));
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update()
{
	for(auto& object : m_objects)
	{
		// Only update root objects (objects without a parent)
		// Children will be updated by their parents
		if (object->GetParent() == nullptr)
		{
			object->Update();
		}
	}
}

void Scene::LateUpdate()
{
	// Remove objects that are marked for deletion after all updates are done
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[](const auto& object) { return object->IsMarkedForDeletion(); }
		),
		m_objects.end()
	);
}

void Scene::Render() const
{
    std::vector<GameObject*> sortedObjects{};
	sortedObjects.reserve(m_objects.size());

	for (const auto& object : m_objects)
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

	return;

	for (const auto& object : m_objects)
	{
		// Only render root objects (objects without a parent)
		// Children will be rendered by their parents
		if (object->GetParent() == nullptr)
		{
			object->Render();
		}
	}
}

