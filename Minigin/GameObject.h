#pragma once
#include <memory>
#include <vector>
#include <typeindex>
#include <unordered_map>

namespace dae
{
	class BaseComponent;

	class GameObject final
	{
	public:
		GameObject() = default;
		~GameObject();

		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;


		void Update(float deltaTime);
		void Render() const;


		// Component template management
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args);

		template<typename T>
		void RemoveComponent();

		template<typename T>
		T* GetComponent() const;

		template<typename T>
		bool HasComponent() const;

	private:
		std::vector<std::unique_ptr<BaseComponent>> m_components{};
		std::unordered_map<std::type_index, BaseComponent*> m_componentMap{};
	};


	// Component template implementations
	template<typename T, typename... Args>
	T* GameObject::AddComponent(Args&&... args)
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "T must derive from BaseComponent");

		// Check if component already exists
		if (HasComponent<T>())
		{
			return GetComponent<T>();
		}

		// Create new component
		auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
		T* componentPtr = component.get();

		// Store in both containers
		m_components.push_back(std::move(component));
		m_componentMap[std::type_index(typeid(T))] = componentPtr;

		return componentPtr;
	}

	template<typename T>
	void GameObject::RemoveComponent()
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "T must derive from BaseComponent");

		const auto typeIndex = std::type_index(typeid(T));

		// Remove from map
		auto mapIt = m_componentMap.find(typeIndex);
		if (mapIt == m_componentMap.end())
			return;

		BaseComponent* componentPtr = mapIt->second;
		m_componentMap.erase(mapIt);

		// Remove from vector
		m_components.erase(
			std::remove_if(
				m_components.begin(),
				m_components.end(),
				[componentPtr](const std::unique_ptr<BaseComponent>& comp)
				{
					return comp.get() == componentPtr;
				}
			),
			m_components.end()
		);
	}

	template<typename T>
	T* GameObject::GetComponent() const
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "T must derive from BaseComponent");

		const auto it = m_componentMap.find(std::type_index(typeid(T)));
		if (it != m_componentMap.end())
		{
			return static_cast<T*>(it->second);
		}
		return nullptr;
	}

	template<typename T>
	bool GameObject::HasComponent() const
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "T must derive from BaseComponent");
		return m_componentMap.find(std::type_index(typeid(T))) != m_componentMap.end();
	}
}
