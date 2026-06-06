#pragma once

#include "Event.h"
#include "../Scene/GameObject.h"

namespace dae
{
	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void Notify(GameObject& actor, Event event) = 0;
	};
}
