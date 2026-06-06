#pragma once
#include "EventQueue/IObserver.h"
#include "Scene/Scene.h"

namespace dae
{
	class EntityDeathObserver final : public IObserver
	{
	public:
		explicit EntityDeathObserver(Scene&);
		~EntityDeathObserver() override;

		void Notify(GameObject& actor, Event event) override;

	private:
		Scene* m_pScene{};
	};
}
