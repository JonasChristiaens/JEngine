#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update()
{
	for (size_t i = 0; i < m_Scenes.size(); ++i)
	{
		m_Scenes[i]->Update();
	}

	for (size_t i = 0; i < m_Scenes.size(); ++i)
	{
		m_Scenes[i]->LateUpdate();
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_Scenes)
	{
		scene->Render();
	}
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_Scenes.emplace_back(new Scene());
	return *m_Scenes.back();
}

void dae::SceneManager::RemoveAll()
{
	m_Scenes.clear();
}
