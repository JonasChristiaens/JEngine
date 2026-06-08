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
	++m_ObjectVersion;
}

void Scene::RemoveAll()
{
	m_Objects.clear();
	++m_ObjectVersion;
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

bool Scene::IsEmpty() const
{
	return m_Objects.empty();
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
	const size_t before = m_Objects.size();

	m_Objects.erase(
		std::remove_if(
			m_Objects.begin(),
			m_Objects.end(),
			[](const auto& object) { return object->IsMarkedForDeletion(); }
		),
		m_Objects.end()
	);

	if (m_Objects.size() != before)
		++m_ObjectVersion;
}

void Scene::Render() const
{
	if (m_ObjectVersion != m_LastSortedVersion)
	{
		m_SortedRoots.clear();
		m_SortedRoots.reserve(m_Objects.size());

		for (const auto& object : m_Objects)
		{
			if (object->GetParent() == nullptr)
			{
				m_SortedRoots.push_back(object.get());
			}
		}

		std::stable_sort(m_SortedRoots.begin(), m_SortedRoots.end(), [](const GameObject* lhs, const GameObject* rhs)
		{
			const auto* lhsRender = lhs->GetComponent<RenderComponent>();
			const auto* rhsRender = rhs->GetComponent<RenderComponent>();

			const int lhsLayer = lhsRender ? lhsRender->GetRenderLayer() : 0;
			const int rhsLayer = rhsRender ? rhsRender->GetRenderLayer() : 0;

			return lhsLayer < rhsLayer;
		});

		m_LastSortedVersion = m_ObjectVersion;
	}

	for (const auto* object : m_SortedRoots)
	{
		object->Render();
	}
}

}
