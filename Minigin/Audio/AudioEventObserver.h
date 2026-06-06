#pragma once
#include "EventQueue/IObserver.h"

namespace dae
{
	class AudioEventObserver final : public IObserver
	{
	public:
		AudioEventObserver();
		~AudioEventObserver() override;

		void Notify(GameObject& actor, Event event) override;
	};
}
