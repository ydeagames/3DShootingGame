#pragma once
#include "ISceneBuilder.h"

// シーン情報
struct SceneInfo
{
	std::string name;
	std::unique_ptr<Scene> scene;
};

// シーンマネージャー
class SceneManager
{
private:
	std::unordered_map<std::string, std::unique_ptr<ISceneBuilder>> m_sceneBuilders;
	std::queue<std::string> m_loadQueue;
	std::unique_ptr<SceneInfo> m_sceneInfo;

public:
	SceneManager();

public:
	inline const SceneInfo& GetActiveScene() const
	{
		return *m_sceneInfo;
	}

	void Register(const std::string& name, std::unique_ptr<ISceneBuilder>&& sceneBuilder);

	template<typename T, typename... Args>
	void Register(const std::string& name, Args... args)
	{
		Register(name, std::make_unique<T>(args...));
	}

	void LoadScene(const std::string& name);
	void ProcessScene(GameContext& context);
};
