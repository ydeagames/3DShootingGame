#include "pch.h"
#include "PauseHandler.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/ECS/Scene.h>

namespace
{
	bool ends_with(const std::wstring& s, const std::wstring& suffix) {
		if (s.size() < suffix.size()) return false;
		return std::equal(std::rbegin(suffix), std::rend(suffix), std::rbegin(s));
	}

	std::wstring substring_before(const std::wstring& s, const std::wstring& suffix)
	{
		return s.substr(0, s.find(suffix));
	}
}

void PauseHandler::SetPaused(bool pause)
{
	auto& manager = context.GetSceneManager();

	auto paused = ends_with(manager.GetActiveScene().name, L"Pause");
	if (paused == pause)
		return;

	if (paused)
	{
		//bool found = false;
		//auto foundname = std::wstring();

		auto& sceneview = manager.GetSceneView();
		for (auto& scene : sceneview.scenes)
			if (ends_with(scene->name, L"Pause"))
			{
				//if (!found)
				//{
				//	found = true;
				//	foundname = substring_before(scene->name, L"Pause");
				//}
				Object::Destroy(*scene);
			}
	}
	else
	{
		auto& scene = manager.GetActiveScene();
		auto name = scene.name + L"Pause";
		if (manager.IsSceneValid(name))
			manager.LoadScene(name, LoadSceneMode::Additive);
	}

	paused = pause;
}
