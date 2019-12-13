#include "pch.h"
#include "SceneManager.h"
#include <Framework/ECS/GameContext.h>

SceneManager::SceneManager()
{
	scenes.emplace_back(std::make_unique<Scene>());
}

int SceneManager::GetSceneCount()
{
	return scenes.size();
}

Scene* SceneManager::GetSceneOrNull(void* sceneptr)
{
	auto itr = std::find_if(scenes.begin(), scenes.end(), [&](auto& scene) { return scene.get() == sceneptr; });
	if (itr == scenes.end())
		return nullptr;
	return (*itr).get();
}

Scene& SceneManager::GetActiveScene() const
{
	return *scenes.front();
}

void SceneManager::Apply()
{
	while (!m_loadQueue.empty())
	{
		auto& task = m_loadQueue.front();
		{
			if (task.mode == LoadSceneMode::Single && !scenes.empty())
				scenes.pop_front();
			scenes.push_front(std::make_unique<Scene>(task.scene));
			scenes.front()->Load();
		}
		m_loadQueue.pop();
	}

	scenes.erase(std::remove_if(scenes.begin(), scenes.end(), [](auto& scene)->bool { return scene->destroyed; }), scenes.end());
}

void SceneManager::LoadScene(const SceneInfo& info, LoadSceneMode mode)
{
	m_loadQueue.push(LoadSceneInfo{ info, mode });
}
