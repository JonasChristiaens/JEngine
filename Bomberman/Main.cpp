#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Core/Minigin.h"
#include "Scene/SceneManager.h"
#include "Resources/ResourceManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Input/InputManager.h"
#include "Input/ControllerButtons.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TextComponent.h"
#include "Components/FPSComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/PlayerAnimatorComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/DisplayComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/PickupComponent.h"
#include "Commands/MoveCommand.h"
#include "Commands/ChangeHealthCommand.h"
#include "Commands/ChangeScoreCommand.h"
#include "Commands/SpawnBombCommand.h"
#include "Observers/BombEventObserver.h"
#include "Audio/AudioEventObserver.h"

#if USE_STEAMWORKS
#include "Commands/ResetAchievementsCommand.h"
#endif

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	// ============
	// Window Setup
	// ============
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	static std::unique_ptr<dae::BombEventObserver> g_BombObserver{};
	static std::unique_ptr<dae::AudioEventObserver> g_AudioObserver{};

	// Background
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::TransformComponent>();
	auto render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("background.png");
	scene.Add(std::move(go));

	// Logo
	go = std::make_unique<dae::GameObject>();
	auto transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(358, 180);
	render = go->AddComponent<dae::RenderComponent>();	
	render->SetTexture("logo.png");
	scene.Add(std::move(go));

	// Text
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(292, 20);
	auto text = go->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
	text->SetColor({ 255, 255, 255, 255 });
	scene.Add(std::move(go));

	// FPS Counter
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(10, 10);
	text = go->AddComponent<dae::TextComponent>("0.0 FPS", font);
	text->SetColor({ 255, 255, 255, 255 });
	go->AddComponent<dae::FPSComponent>();
	scene.Add(std::move(go));


	// ============
	// Player Setup
	// ============
	// Player 1 (WASD controls)
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(100, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetSpriteSheet(16, 16, 6, 2);
	render->SetSprite(4, 0);
	render->SetScale(2.0f);
	render->SetRenderLayer(5);
	go->AddComponent<dae::SpriteAnimatorComponent>();
	go->AddComponent<dae::PlayerAnimatorComponent>();
	go->AddComponent<dae::HealthComponent>(3);
	go->AddComponent<dae::ScoreComponent>(0);
	go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	auto player1 = go.get();
	scene.Add(std::move(go));

	// Player 2 (DPad controls)
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(200, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetSpriteSheet(16, 16, 6, 2);
	render->SetSprite(4, 0);
	render->SetScale(2.0f);
   render->SetRenderLayer(5);
	go->AddComponent<dae::SpriteAnimatorComponent>();
	go->AddComponent<dae::PlayerAnimatorComponent>();
	go->AddComponent<dae::HealthComponent>(3);
	go->AddComponent<dae::ScoreComponent>(0);
	go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	auto player2 = go.get();
	scene.Add(std::move(go));
	

	// ==========================
	// Pickup Setup
	// ==========================
	// Pickup worth 10
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(300, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetScale(2.0f);
	render->SetSourceRectangle(0, 224, 16, 16);
	auto pickupCollider = go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	auto pickupComp = go->AddComponent<dae::PickupComponent>(10);
	pickupCollider->SetOnCollisionCallback([pickupComp](dae::GameObject* other) {
		pickupComp->OnCollision(other);
	});
	scene.Add(std::move(go));

	// Pickups worth 100
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(350, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetScale(2.0f);
	render->SetSourceRectangle(0, 224, 16, 16);
	pickupCollider = go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	pickupComp = go->AddComponent<dae::PickupComponent>(100);
	pickupCollider->SetOnCollisionCallback([pickupComp](dae::GameObject* other) {
		pickupComp->OnCollision(other);
		});
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(400, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetScale(2.0f);
	render->SetSourceRectangle(0, 224, 16, 16);
	pickupCollider = go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	pickupComp = go->AddComponent<dae::PickupComponent>(100);
	pickupCollider->SetOnCollisionCallback([pickupComp](dae::GameObject* other) {
		pickupComp->OnCollision(other);
		});
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(450, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetScale(2.0f);
	render->SetSourceRectangle(0, 224, 16, 16);
	pickupCollider = go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	pickupComp = go->AddComponent<dae::PickupComponent>(100);
	pickupCollider->SetOnCollisionCallback([pickupComp](dae::GameObject* other) {
		pickupComp->OnCollision(other);
		});
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(500, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetScale(2.0f);
	render->SetSourceRectangle(0, 224, 16, 16);
	pickupCollider = go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	pickupComp = go->AddComponent<dae::PickupComponent>(100);
	pickupCollider->SetOnCollisionCallback([pickupComp](dae::GameObject* other) {
		pickupComp->OnCollision(other);
		});
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(550, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Textures/BombermanSprites_General.png");
	render->SetScale(2.0f);
	render->SetSourceRectangle(0, 224, 16, 16);
	pickupCollider = go->AddComponent<dae::CollisionComponent>(32.0f, 32.0f);
	pickupComp = go->AddComponent<dae::PickupComponent>(100);
	pickupCollider->SetOnCollisionCallback([pickupComp](dae::GameObject* other) {
		pickupComp->OnCollision(other);
		});
	scene.Add(std::move(go));



	// ==========================
	// Player Information Display
	// ==========================
	// Instructions
	auto infoFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 18);

	// Bomberman 1 Instructions
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(10, 100);
	text = go->AddComponent<dae::TextComponent>("Use the D-Pad to move Bomberman 1, X to inflict damage, A and B to pick up points", infoFont);
	text->SetColor({ 180, 180, 180, 255 });
	scene.Add(std::move(go));

	// Bomberman 2 Instructions
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(10, 120);
	text = go->AddComponent<dae::TextComponent>("Use WASD to move Bomberman 2, C to inflict damage, Z and X to pick up points", infoFont);
	text->SetColor({ 180, 180, 180, 255 });
	scene.Add(std::move(go));

   go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(10, 140);
	text = go->AddComponent<dae::TextComponent>("Place bombs with R (keyboard) or Y (controller)", infoFont);
	text->SetColor({ 180, 180, 180, 255 });
	scene.Add(std::move(go));

	// Bomberman 1 Lives Display
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
   transform->SetLocalPosition(10, 190);
	text = go->AddComponent<dae::TextComponent>("# lives: 3", infoFont);
	text->SetColor({ 180, 180, 180, 255 });
	auto livesDisplayP1 = go->AddComponent<dae::DisplayComponent<dae::HealthComponent>>(
		text,
		dae::make_sdbm_hash("HealthChanged"),
		"# lives: ",
		[](const dae::HealthComponent* c) { return std::to_string(c->GetHealth()); }
	);
	player1->GetComponent<dae::HealthComponent>()->AddObserver(*livesDisplayP1);
	scene.Add(std::move(go));

	// Bomberman 1 Score Display
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
   transform->SetLocalPosition(10, 210);
	text = go->AddComponent<dae::TextComponent>("Score: 0", infoFont);
	text->SetColor({ 180, 180, 180, 255 });
	auto scoreDisplayP1 = go->AddComponent<dae::DisplayComponent<dae::ScoreComponent>>(
		text,
		dae::make_sdbm_hash("ScoreChanged"),
		"Score: ",
		[](const dae::ScoreComponent* c) { return std::to_string(c->GetScore()); }
	);
	player1->GetComponent<dae::ScoreComponent>()->AddObserver(*scoreDisplayP1);
	scene.Add(std::move(go));

	// Bomberman 2 Lives Display
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
   transform->SetLocalPosition(10, 230);
	text = go->AddComponent<dae::TextComponent>("# lives: 3", infoFont);
	text->SetColor({ 180, 180, 180, 255 });
	auto livesDisplayP2 = go->AddComponent<dae::DisplayComponent<dae::HealthComponent>>(
		text,
		dae::make_sdbm_hash("HealthChanged"),
		"# lives: ",
		[](const dae::HealthComponent* c) { return std::to_string(c->GetHealth()); }
	);
	player2->GetComponent<dae::HealthComponent>()->AddObserver(*livesDisplayP2);
	scene.Add(std::move(go));

	// Bomberman 2 Score Display
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
   transform->SetLocalPosition(10, 250);
	text = go->AddComponent<dae::TextComponent>("Score: 0", infoFont);
	text->SetColor({ 180, 180, 180, 255 });
	auto scoreDisplayP2 = go->AddComponent<dae::DisplayComponent<dae::ScoreComponent>>(
		text,
		dae::make_sdbm_hash("ScoreChanged"),
		"Score: ",
		[](const dae::ScoreComponent* c) { return std::to_string(c->GetScore()); }
	);
	player2->GetComponent<dae::ScoreComponent>()->AddObserver(*scoreDisplayP2);
	scene.Add(std::move(go));


	// =======================
	// Input Commands bindings
	// =======================
	// Input binding
	const float moveSpeed = 150.0f;
	auto& input = dae::InputManager::GetInstance();

	// Player 1 - WASD keyboard controls
	auto moveUpP1 = std::make_unique<MoveCommand>(glm::vec3(0, -1, 0), moveSpeed);
	moveUpP1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_W, dae::KeyState::Pressed, std::move(moveUpP1));

	auto moveDownP1 = std::make_unique<MoveCommand>(glm::vec3(0, 1, 0), moveSpeed);
	moveDownP1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_S, dae::KeyState::Pressed, std::move(moveDownP1));

	auto moveLeftP1 = std::make_unique<MoveCommand>(glm::vec3(-1, 0, 0), moveSpeed);
	moveLeftP1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_A, dae::KeyState::Pressed, std::move(moveLeftP1));

	auto moveRightP1 = std::make_unique<MoveCommand>(glm::vec3(1, 0, 0), moveSpeed);
	moveRightP1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_D, dae::KeyState::Pressed, std::move(moveRightP1));

	auto damageP1 = std::make_unique<ChangeHealthCommand>(-1);
	damageP1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_C, dae::KeyState::Down, std::move(damageP1));

	auto score10P1 = std::make_unique<ChangeScoreCommand>(10);
	score10P1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_Z, dae::KeyState::Down, std::move(score10P1));

	auto score100P1 = std::make_unique<ChangeScoreCommand>(100);
	score100P1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_X, dae::KeyState::Down, std::move(score100P1));

	auto spawnBombP1 = std::make_unique<SpawnBombCommand>();
	spawnBombP1->SetGameActor(player1);
	input.BindKeyboardInput(SDLK_R, dae::KeyState::Down, std::move(spawnBombP1));


	// Player 2 - Controller DPad controls
	const unsigned int controllerIndex = 0;
	input.AddController(controllerIndex);

	auto moveUpP2 = std::make_unique<MoveCommand>(glm::vec3(0, -1, 0), moveSpeed * 2);
	moveUpP2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::DPadUp, dae::KeyState::Pressed, std::move(moveUpP2));

	auto moveDownP2 = std::make_unique<MoveCommand>(glm::vec3(0, 1, 0), moveSpeed * 2);
	moveDownP2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::DPadDown, dae::KeyState::Pressed, std::move(moveDownP2));

	auto moveLeftP2 = std::make_unique<MoveCommand>(glm::vec3(-1, 0, 0), moveSpeed * 2);
	moveLeftP2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::DPadLeft, dae::KeyState::Pressed, std::move(moveLeftP2));

	auto moveRightP2 = std::make_unique<MoveCommand>(glm::vec3(1, 0, 0), moveSpeed * 2);
	moveRightP2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::DPadRight, dae::KeyState::Pressed, std::move(moveRightP2));

	auto damageP2 = std::make_unique<ChangeHealthCommand>(-1);
	damageP2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::X, dae::KeyState::Down, std::move(damageP2));

	auto score10P2 = std::make_unique<ChangeScoreCommand>(10);
	score10P2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::A, dae::KeyState::Down, std::move(score10P2));

	auto score100P2 = std::make_unique<ChangeScoreCommand>(100);
	score100P2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::B, dae::KeyState::Down, std::move(score100P2));

	auto spawnBombP2 = std::make_unique<SpawnBombCommand>();
	spawnBombP2->SetGameActor(player2);
	input.BindControllerInput(controllerIndex, ControllerButton::Y, dae::KeyState::Down, std::move(spawnBombP2));

#if USE_STEAMWORKS
	// Utility - Reset Achievements
	auto resetAchievements = std::make_unique<ResetAchievementsCommand>();
	input.BindKeyboardInput(SDLK_F1, dae::KeyState::Down, std::move(resetAchievements));
#endif

	if (!g_BombObserver)
		g_BombObserver = std::make_unique<dae::BombEventObserver>(scene);

	if (!g_AudioObserver)
		g_AudioObserver = std::make_unique<dae::AudioEventObserver>();
}


int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if (!fs::exists(data_location))
		data_location = "../Data/";
#endif

	dae::Minigin engine(data_location);
	engine.Run(load);
	return 0;
}
