#pragma once

namespace dae
{
	class GameObject;
	
	class BaseComponent
	{
	public:
		BaseComponent(GameObject* pOwner);
		virtual ~BaseComponent() = default;

		BaseComponent(const BaseComponent& other) = delete;
		BaseComponent(BaseComponent&& other) = delete;
		BaseComponent& operator=(const BaseComponent& other) = delete;
		BaseComponent& operator=(BaseComponent&& other) = delete;

		virtual void Update(float deltaTime) = 0;
		virtual void Render() const {};

		GameObject* GetOwner() const { return m_pOwner; }

	protected:
		GameObject* m_pOwner;
	};
}