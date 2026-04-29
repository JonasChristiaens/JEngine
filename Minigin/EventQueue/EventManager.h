#pragma once
#include "../Core/Singleton.h"
#include "ISubject.h"
#include <queue>
#include <mutex>

namespace dae
{
	class EventManager final : public Singleton<EventManager>, public ISubject
	{
	public:
       static bool IsAlive() { return s_isAlive; }
		void BroadcastEvent(Event e, GameObject* pSubjectActor) { QueueEvent(e, pSubjectActor); }

		void QueueEvent(Event e, GameObject* pSubjectActor)
		{
			std::lock_guard lock(m_QueueMutex);
			m_Queue.push({ e, pSubjectActor });
		}

		void ProcessQueuedEvents()
		{
			std::queue<QueuedEvent> local;
			{
				std::lock_guard lock(m_QueueMutex);
				std::swap(local, m_Queue);
			}

			while (!local.empty())
			{
				const auto& qe = local.front();
				NotifyObservers(qe.event, qe.subject);
				local.pop();
			}
		}

	private:
		friend class Singleton<EventManager>;
       EventManager() { s_isAlive = true; }
		~EventManager() { s_isAlive = false; }

		inline static bool s_isAlive{ false };

		struct QueuedEvent
		{
			Event event;
			GameObject* subject{};
		};

		std::queue<QueuedEvent> m_Queue{};
		std::mutex m_QueueMutex{};
	};
}
