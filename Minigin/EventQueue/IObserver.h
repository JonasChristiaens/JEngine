#pragma once
#include "../Scene/GameObject.h"
#include "Event.h"

namespace dae
{
	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void Notify(const GameObject& pGameActor, Event event) = 0;
	};
}