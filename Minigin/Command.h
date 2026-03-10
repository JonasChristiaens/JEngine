#pragma once

namespace dae
{
	class GameObject;
}

class Command
{
public:
	virtual ~Command() = default;
	virtual void Execute() = 0;

	void SetGameActor(dae::GameObject* pGameActor) { m_pGameActor = pGameActor; }
	dae::GameObject* GetGameActor() const { return m_pGameActor; }

protected:
	dae::GameObject* m_pGameActor = nullptr;
};