#pragma once
#include "../Singleton.h"
#include "ISubject.h"

namespace dae
{
	class EventManager final : public Singleton<EventManager>, public ISubject
	{
	public:
		void BroadcastEvent(Event e, GameObject* pSubjectActor)
		{
			NotifyObservers(e, pSubjectActor);
		}
	};
}
