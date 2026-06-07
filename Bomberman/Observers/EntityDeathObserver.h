#pragma once
#include "EventQueue/IObserver.h"

namespace dae
{
	class Scene;

	class EntityDeathObserver final : public IObserver
	{
	public:
		explicit EntityDeathObserver(Scene& scene);
		~EntityDeathObserver() override;

		void Notify(GameObject& actor, Event event) override;

	private:
		Scene* m_pScene{};
	};
}
