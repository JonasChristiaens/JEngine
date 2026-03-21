#pragma once

#include "IObserver.h"
#include <vector>

namespace dae
{
	class ISubject
	{
	public:
		void AddObserver(IObserver& observer) { m_observers.emplace_back(&observer); }
		void RemoveObserver(IObserver& observer) {
			m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), &observer), m_observers.end());
		}

	protected:
		void NotifyObservers(Event e, GameObject* pSubjectActor) {
			for (auto observer : m_observers)
				observer->Notify(*pSubjectActor, e);
		}

	private:
		std::vector<IObserver*> m_observers;
	};
}