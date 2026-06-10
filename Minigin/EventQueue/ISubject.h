#pragma once

#include "IObserver.h"
#include <vector>

namespace dae
{
	class ISubject
	{
	public:
		virtual ~ISubject() = default;

		void AddObserver(IObserver& observer) { m_Observers.emplace_back(&observer); }
		void RemoveObserver(IObserver& observer) {
			m_Observers.erase(std::remove(m_Observers.begin(), m_Observers.end(), &observer), m_Observers.end());
		}

	protected:
		void NotifyObservers(Event e, GameObject* pSubjectActor) {
			for (auto observer : m_Observers)
			{
				if (pSubjectActor)
					observer->Notify(*pSubjectActor, e);
			}
		}

	private:
		std::vector<IObserver*> m_Observers;
	};
}
