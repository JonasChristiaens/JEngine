#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "InputManager.h"
#include "MoveCommand.h"
#include "SpriteAnimatorComponent.h"
#include "ControllerButtons.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

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
	text->SetColor({ 255, 255, 0, 255 });
	scene.Add(std::move(go));

	// FPS Counter
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(10, 10);
	text = go->AddComponent<dae::TextComponent>("0.0 FPS", font);
	text->SetColor({ 255, 255, 255, 255 });
	go->AddComponent<dae::FPSComponent>();
	scene.Add(std::move(go));


	// Player 1 (WASD controls)
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(100, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Resources/BombermanSprites_General.png");
	render->SetSpriteSheet(16, 16, 6, 2);
	render->SetSprite(4, 0);
	render->SetScale(2.0f);
	auto animator = go->AddComponent<dae::SpriteAnimatorComponent>();
	animator->SetAnimationSpeed(10.0f);
	auto player1 = go.get();
	scene.Add(std::move(go));

	// Player 2 (DPad controls)
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(200, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("Resources/BombermanSprites_General.png");
	render->SetSpriteSheet(16, 16, 6, 2);
	render->SetSprite(4, 0);
	render->SetScale(2.0f);
	animator = go->AddComponent<dae::SpriteAnimatorComponent>();
	animator->SetAnimationSpeed(10.0f);
	auto player2 = go.get();
	scene.Add(std::move(go));


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
}


int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
	return 0;
}
