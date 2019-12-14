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
	int GetSceneCount();
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

	void Apply();
	void LoadScene(const SceneInfo& info, LoadSceneMode mode = LoadSceneMode::Single);
};
