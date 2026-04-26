#pragma once
#include "Command.h"

class SpawnBombCommand final : public Command
{
public:
    void Execute() override;
};