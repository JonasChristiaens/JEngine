#pragma once

namespace dae
{
	enum class Event
	{
		PlayerDamaged,
		PlayerScoreSmallChanged,
		PlayerScoreLargeChanged,
		PlayerWonGame
	};

	class GameObject;

	class Observer
	{
	public:
		virtual ~Observer() = default;
		virtual void Notify(Event e, GameObject* pSubjectActor) = 0;

	protected:
		GameObject* m_pGameActor = nullptr;
	};
}