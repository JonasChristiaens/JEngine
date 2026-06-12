#pragma once
#include "../Core/Singleton.h"
#include "Subject.h"
#include <queue>
#include <mutex>

namespace dae
{
	class EventManager final : public Singleton<EventManager>, public Subject
	{
	public:
		void BroadcastEvent(Event e, GameObject* pSubjectActor) { QueueEvent(e, pSubjectActor); }
		void BroadcastImmediate(Event e, GameObject* pSubjectActor) { NotifyObservers(e, pSubjectActor); }

		void QueueEvent(Event e, GameObject* pSubjectActor)
		{
			std::lock_guard lock(m_QueueMutex);
			m_Queue.push({ e, pSubjectActor });
		}

		void ClearQueue()
		{
			std::lock_guard lock(m_QueueMutex);
			std::queue<QueuedEvent> empty;
			std::swap(m_Queue, empty);
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
		EventManager() = default;
		~EventManager() = default;

		struct QueuedEvent
		{
			Event event;
			GameObject* subject{};
		};

		std::queue<QueuedEvent> m_Queue{};
		std::mutex m_QueueMutex{};
	};
}
