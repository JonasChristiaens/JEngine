#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Core/Minigin.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/SceneStateMachineComponent.h"
#include "State/TitleSceneState.h"
#include "HighScoreManager.h"

#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

namespace
{
	void load()
	{
		dae::HighScoreManager::LoadFromFile("Data/highscores.json");

		auto& scene = dae::SceneManager::GetInstance().CreateScene();
		auto stateMachineObject = std::make_unique<dae::GameObject>();
		auto* stateMachineComponent = stateMachineObject->AddComponent<dae::SceneStateMachineComponent>();
		scene.Add(std::move(stateMachineObject));

		stateMachineComponent->GetStateMachine().SetState(std::make_unique<dae::TitleSceneState>(*stateMachineComponent));
	}
}


int main(int, char* []) {
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