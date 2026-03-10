#pragma once
#include "Command.h"
#include <iostream>

class ExampleCommand : public Command
{
public:
	void Execute() override
	{
		std::cout << "Example Command Executed!\n";
	}
};
