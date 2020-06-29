// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 20:52:53 +0900
// Modified: 2020-01-09 18:30:53 +0900

#pragma once
#include <Framework/ECS/Scene.h>

// シーンオプション
enum class LoadSceneMode
{
	Single,
	Additive,
};

// シーンマネージャー
class SceneManager
{
private:
	// シーンロードタスク
	class LoadSceneInfo
	{
	public:
		SceneInfo scene;
		LoadSceneMode mode;
	};

private:
	std::queue<LoadSceneInfo> m_loadQueue;
	std::deque<std::unique_ptr<Scene>> scenes;

public:
	SceneManager();

public:
	int GetSceneCount() const;
	std::deque<std::unique_ptr<Scene>>& GetScenes();
	Scene* GetSceneOrNull(void* sceneptr);
	Scene* GetSceneOrNullRegistry(void* sceneptr);
	Scene& GetActiveScene() const;

	template <typename Func>
	void ForEachScenes(Func func) const
	{
		for (auto& scene : scenes)
			func(*scene);
	}

	template <typename Func>
	void ForEachScenesInverted(Func func) const
	{
		for (auto itr = scenes.rbegin(); itr != scenes.rend(); ++itr)
			func(*(*itr));
	}

	using Transition = std::function<void(const SceneInfo& info, entt::registry& reg)>;

	void Apply();
	void LoadScene(const SceneInfo& info, LoadSceneMode mode = LoadSceneMode::Single);
	void LoadSceneTransition(const SceneInfo& info, const Transition& transition);
};


