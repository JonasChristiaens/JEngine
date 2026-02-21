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
#include "MoveComponent.h"

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


	// First Bomberman character
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(300, 300);
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("../Resources/BombermanSprites_General.png");
	render->SetSpriteSheet(16, 16, 6, 2);
	render->SetSprite(4, 0);
	render->SetScale(3.0f);
	auto move = go->AddComponent<dae::MoveComponent>(100.0f, 2.0f); // radius: 100, speed: 1 rad/s
	move->SetRotationCenter(300, 300); // Rotate around this point
	auto* pBomberman = go.get();
	scene.Add(std::move(go));

	// Second Bomberman character orbiting around the first
	go = std::make_unique<dae::GameObject>();
	transform = go->AddComponent<dae::TransformComponent>();
	transform->SetLocalPosition(0, 0); // Local position relative to parent
	render = go->AddComponent<dae::RenderComponent>();
	render->SetTexture("../Resources/BombermanSprites_General.png");
	render->SetSpriteSheet(16, 16, 6, 2);
	render->SetSprite(0, 0);
	render->SetScale(3.0f);
	go->AddComponent<dae::MoveComponent>(50.0f, 2.0f); // radius: 50, speed: 2 rad/s
	auto* pChild = go.get();
	scene.Add(std::move(go));
	pChild->SetParent(pBomberman, false); // Set parent-child relationship
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
