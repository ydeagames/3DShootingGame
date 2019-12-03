#include "pch.h"
#include "SceneManager.h"
#include <Framework/ECS/GameContext.h>

SceneManager::SceneManager(Scene& scene)
	: scene(&scene)
{
}

Scene& SceneManager::GetActiveScene()
{
	return *scene;
}

void SceneManager::Apply()
{
	if (requested)
	{
		requested = false;
		auto& scene = GameContext::Get<Scene>();
		scene.info = nextScene;
		scene.Load();
	}
}

void SceneManager::LoadScene(const SceneInfo& info)
{
	nextScene = info;
	requested = true;
}
