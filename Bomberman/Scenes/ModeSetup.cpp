#include "ModeSetup.h"
#include "PlayerSetup.h"
#include "Components/PlayfieldComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/EnemyComponent.h"
#include "Factories/EnemyFactory.h"
#include "Config/EnemyConfig.h"
#include "Input/InputManager.h"
#include <algorithm>

namespace
{
	constexpr float kPlayfieldWidth{ 496.0f };
	constexpr float kPlayfieldHeight{ 208.0f };
	constexpr float kBalloomSpeed{ 90.0f };
	constexpr float kOnealSpeed{ 120.0f };
	constexpr float kDollSpeed{ 120.0f };
	constexpr float kMinvoSpeed{ 180.0f };

	const dae::EnemyConfig kBalloomConfig = []()
		{
			dae::EnemyConfig config{
				0.0f, 240.0f, 16.0f, 16.0f,
				3.0f, 0.90f,
				2.0f, 4.0f,
				dae::EnemyChaseAxis::None,
				100
			};
			config.deathFrames = dae::BuildHorizontalFrames(96.0f, 240.0f, 1, 16.0f, 16.0f);
			auto orangeDeath = dae::BuildHorizontalFrames(112.0f, 240.0f, 4, 16.0f, 16.0f);
			config.deathFrames.insert(config.deathFrames.end(), orangeDeath.begin(), orangeDeath.end());
			config.deathFps = 10.0f;
			return config;
		}();

	const dae::EnemyConfig kOnealConfig = []()
		{
			dae::EnemyConfig config{
				0.0f, 256.0f, 16.0f, 16.0f,
				3.0f, 0.90f,
				0.8f, 3.2f,
				dae::EnemyChaseAxis::Y,
				200
			};
			config.deathFrames = dae::BuildHorizontalFrames(96.0f, 256.0f, 1, 16.0f, 16.0f);
			auto blueDeath = dae::BuildHorizontalFrames(112.0f, 288.0f, 4, 16.0f, 16.0f);
			config.deathFrames.insert(config.deathFrames.end(), blueDeath.begin(), blueDeath.end());
			config.deathFps = 10.0f;
			return config;
		}();

	const dae::EnemyConfig kDollConfig = []()
		{
			dae::EnemyConfig config{
				0.0f, 272.0f, 16.0f, 16.0f,
				3.0f, 0.90f,
				0.8f, 3.2f,
				dae::EnemyChaseAxis::X,
				400
			};
			config.deathFrames = dae::BuildHorizontalFrames(96.0f, 272.0f, 1, 16.0f, 16.0f);
			auto deathRest = dae::BuildHorizontalFrames(112.0f, 272.0f, 4, 16.0f, 16.0f);
			config.deathFrames.insert(config.deathFrames.end(), deathRest.begin(), deathRest.end());
			config.deathFps = 10.0f;
			return config;
		}();

	const dae::EnemyConfig kMinvoConfig = []()
		{
			dae::EnemyConfig config{
				0.0f, 288.0f, 16.0f, 16.0f,
				3.0f, 0.90f,
				0.5f, 2.0f,
				dae::EnemyChaseAxis::Both,
				800
			};
			config.deathFrames = dae::BuildHorizontalFrames(96.0f, 288.0f, 1, 16.0f, 16.0f);
			auto orangeDeath2 = dae::BuildHorizontalFrames(112.0f, 240.0f, 4, 16.0f, 16.0f);
			config.deathFrames.insert(config.deathFrames.end(), orangeDeath2.begin(), orangeDeath2.end());
			config.deathFps = 10.0f;
			return config;
		}();
}

namespace dae
{
	GameObject* SpawnEnemy(Scene& scene, GameObject& parent, float tileWorldSize, const EnemyConfig& config, float moveSpeed,
		const std::vector<GameObject*>& chaseTargets, const glm::vec3& reservedWorldPosition, bool useAiMovement,
		int maxColumn)
	{
		const int gridColumns = static_cast<int>(kPlayfieldWidth / 16.0f);
		const int gridRows = static_cast<int>(kPlayfieldHeight / 16.0f);
		auto* enemy = EnemyFactory::CreateEnemy(scene, parent, gridColumns, gridRows, tileWorldSize, moveSpeed, config, chaseTargets, { reservedWorldPosition }, useAiMovement, maxColumn);
		if (enemy)
		{
			if (auto* playfield = parent.GetComponent<PlayfieldComponent>())
				playfield->RegisterEnemySpawned();
		}
		return enemy;
	}

	std::pair<GameObject*, GameObject*> SetupSoloMode(const SpawnParams& p)
	{
		auto* p1 = CreatePlayer(p.scene, p.player1Pos, p.carryOver);
		p1->SetParent(&p.worldRoot, false);
		AssignPlayerInputs({ p1 });

		const std::vector<GameObject*> chaseTargets{ p1 };

		for (int i = 0; i < p.balloomCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kBalloomConfig, kBalloomSpeed, {}, p.player1Pos, true);
		for (int i = 0; i < p.onealCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kOnealConfig, kOnealSpeed, chaseTargets, p.player1Pos, true);
		for (int i = 0; i < p.dollCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kDollConfig, kDollSpeed, chaseTargets, p.player1Pos, true);
		for (int i = 0; i < p.minvoCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kMinvoConfig, kMinvoSpeed, chaseTargets, p.player1Pos, true);

		return { p1, nullptr };
	}

	std::pair<GameObject*, GameObject*> SetupCoopMode(const SpawnParams& p)
	{
		GameObject* p1 = nullptr;
		GameObject* p2 = nullptr;

		if (p.carryOver.health > 0)
		{
			p1 = CreatePlayer(p.scene, p.player1Pos, p.carryOver);
			p1->SetParent(&p.worldRoot, false);
		}

		if (p.p2CarryOver.health > 0)
		{
			p2 = CreatePlayer(p.scene, p.player2Pos, p.p2CarryOver);
			p2->SetParent(&p.worldRoot, false);
		}

		std::vector<GameObject*> alivePlayers{};
		if (p1) alivePlayers.push_back(p1);
		if (p2) alivePlayers.push_back(p2);
		AssignPlayerInputs(alivePlayers);

		for (int i = 0; i < p.balloomCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kBalloomConfig, kBalloomSpeed, {}, (i == 0) ? p.player1Pos : p.player2Pos, true);
		for (int i = 0; i < p.onealCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kOnealConfig, kOnealSpeed, alivePlayers, (i == 0) ? p.player1Pos : p.player2Pos, true);
		for (int i = 0; i < p.dollCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kDollConfig, kDollSpeed, alivePlayers, (i == 0) ? p.player1Pos : p.player2Pos, true);
		for (int i = 0; i < p.minvoCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kMinvoConfig, kMinvoSpeed, alivePlayers, (i == 0) ? p.player1Pos : p.player2Pos, true);

		return { p1, p2 };
	}

	std::pair<GameObject*, GameObject*> SetupVersusMode(const SpawnParams& p)
	{
		auto* p1 = CreatePlayer(p.scene, p.player1Pos, p.carryOver);
		p1->SetParent(&p.worldRoot, false);

		BindKeyboardMovement(*p1);
		auto& input = InputManager::GetInstance();
		if (input.HasController(1))
			BindControllerMovement(*p1, 1);

		const int gridColumns = static_cast<int>(kPlayfieldWidth / 16.0f);
		const int playerEnemyMaxColumn = gridColumns - 8;

		auto* p2 = SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kBalloomConfig, kBalloomSpeed, {}, p.player1Pos, false, playerEnemyMaxColumn);
		if (p2)
		{
			if (auto* collider = p2->GetComponent<CollisionComponent>())
			{
				collider->SetCollisionFilter([](GameObject* other) {
					return other->HasComponent<EnemyComponent>();
				});
			}
			BindEnemyMovementControls(*p2, 0);
		}

		for (unsigned int ci = 2; ci < 4; ++ci)
		{
			if (!input.HasController(ci))
				continue;

			auto* extra = SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kBalloomConfig, kBalloomSpeed, {}, p.player2Pos, false, playerEnemyMaxColumn);
			if (extra)
			{
				if (auto* collider = extra->GetComponent<CollisionComponent>())
				{
					collider->SetCollisionFilter([](GameObject* other) {
						return other->HasComponent<EnemyComponent>();
					});
				}
				BindEnemyMovementControls(*extra, ci);
			}
		}

		const std::vector<GameObject*> chaseTargets{ p1 };

		for (int i = 0; i < std::max(0, p.balloomCount - 1); ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kBalloomConfig, kBalloomSpeed, {}, p.player2Pos, true);
		for (int i = 0; i < p.onealCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kOnealConfig, kOnealSpeed, chaseTargets, p.player2Pos, true);
		for (int i = 0; i < p.dollCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kDollConfig, kDollSpeed, chaseTargets, p.player2Pos, true);
		for (int i = 0; i < p.minvoCount; ++i)
			SpawnEnemy(p.scene, p.worldRoot, p.tileWorldSize, kMinvoConfig, kMinvoSpeed, chaseTargets, p.player2Pos, true);

		return { p1, p2 };
	}
}
