#include "pch.h"
#include "SceneManager.h"
#include <Framework/ECS/GameContext.h>

void SceneManager::Apply()
{
	GameContext::Get<Scene>().Load
}

void SceneManager::LoadScene(const SceneInfo& info)
{
	nextScene = info;
}
