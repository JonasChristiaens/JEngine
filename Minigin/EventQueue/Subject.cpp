#include "Subject.h"
#include <algorithm>

void dae::Subject::AddObserver(Observer& observer)
{
	m_pObservers.emplace_back(&observer);
}

void dae::Subject::RemoveObserver(Observer& observer)
{
	m_pObservers.erase(std::remove(m_pObservers.begin(), m_pObservers.end(), &observer), m_pObservers.end());
}

void dae::Subject::NotifyObserver(Event e, GameObject* pSubjectActor)
{
	for (Observer* observer : m_pObservers)
	{
		observer->Notify(e, pSubjectActor);
	}
}