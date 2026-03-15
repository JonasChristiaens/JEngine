#pragma once

namespace dae
{
	enum class Event
	{
		HealthChange,
		ScoreChange,
	};

	class GameObject;

	class Observer
	{
	public:
		virtual ~Observer() = default;
		virtual void Notify(Event e, GameObject* m_pGameActor) = 0;

	protected:
		GameObject* m_pGameActor = nullptr;
	};
}