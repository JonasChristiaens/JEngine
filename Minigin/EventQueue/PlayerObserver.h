#pragma once
#include "EventQueue/Observer.h"

namespace dae
{
	class PlayerObserver final : public Observer
	{
	public:
		PlayerObserver() = default;
		virtual ~PlayerObserver() = default;

		void Notify(Event e, GameObject* pSubjectActor) override;
	};
}