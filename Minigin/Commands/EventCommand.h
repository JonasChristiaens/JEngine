#pragma once
#include "Command.h"
#include "EventQueue/Subject.h"

class EventCommand final : public Command, public dae::Subject
{
public:
	explicit EventCommand(dae::Event eventToFire);
	virtual ~EventCommand() = default;

	void Execute() override;

private:
	dae::Event m_event;
};