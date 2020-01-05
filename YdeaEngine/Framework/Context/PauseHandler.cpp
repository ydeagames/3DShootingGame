#include "pch.h"
#include "PauseHandler.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/ECS/Scene.h>
#include <Framework/Context/SceneManager.h>

namespace
{
	bool ends_with(const std::string& s, const std::string& suffix) {
		if (s.size() < suffix.size()) return false;
		return std::equal(std::rbegin(suffix), std::rend(suffix), std::rbegin(s));
	}

	std::string substring_before(const std::string& s, const std::string& suffix)
	{
		return s.substr(0, s.find(suffix));
	}

	std::string substring_after(const std::string& s, const std::string& suffix)
	{
		auto pos = s.find(suffix);
		if (pos != std::string::npos)
			return s.substr(pos + suffix.size());
		return s;
	}
}

void PauseHandler::SetPaused(bool pause)
{
	auto& manager = GameContext::Get<SceneManager>();
	auto& scene = manager.GetActiveScene();

	auto paused = ends_with(scene.info.name, "Pause");
	if (paused == pause)
		return;

	if (paused)
	{
		SceneInfo info = { substring_before(scene.info.name, "Pause"), substring_before(substring_before(scene.info.location, "."), "Pause") + substring_after(scene.info.location, ".") };
		if (manager.GetSceneCount() > 1)
			Scene::Destroy(manager.GetActiveScene());
	}
	else
	{
		SceneInfo info = { scene.info.name + "Pause", substring_before(scene.info.location, ".") + "Pause." + substring_after(substring_after(scene.info.location, ".playing"), ".") };
		if (info.Valid())
		{
			manager.LoadScene(info, LoadSceneMode::Additive);
		}
	}

	paused = pause;
}
