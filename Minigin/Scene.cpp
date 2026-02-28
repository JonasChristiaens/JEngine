#include <algorithm>
#include "Scene.h"
#include <cassert>

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

void Scene::Update(float deltaTime)
{
	for(auto& object : m_objects)
	{
		// Only update root objects (objects without a parent)
		// Children will be updated by their parents
		if (object->GetParent() == nullptr)
		{
			object->Update(deltaTime);
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

