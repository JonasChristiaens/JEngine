#pragma once
#include <memory>
#include <vector>
#include "GameObject.h"

namespace dae
{
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		void RemoveAll();
		bool Contains(const GameObject* object) const;
		bool IsEmpty() const;

		void Update();
		void LateUpdate();
		void Render() const;

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		friend class SceneManager;
		explicit Scene() = default;

		std::vector<std::unique_ptr<GameObject>> m_Objects{};

		size_t m_ObjectVersion{ 0 };
		mutable size_t m_LastSortedVersion{ 0 };
		mutable std::vector<GameObject*> m_SortedRoots{};
	};

}
