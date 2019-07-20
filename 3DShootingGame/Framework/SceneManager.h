#pragma once
#include "ISceneBuilder.h"

// シーン情報
struct SceneInfo
{
	std::wstring name;
	std::unique_ptr<Scene> scene;
};

// シーンオプション
enum class LoadSceneMode
{
	Single,
	Additive,
};

// シーンビュー
struct SceneView : public Component
{
public:
	std::deque<std::unique_ptr<SceneInfo>> scenes;

public:
	// 生成
	void Initialize(GameContext& context);
	// 更新
	void Update(GameContext& context);
	// 描画
	void Render(GameContext& context);
	// 破棄
	void Finalize(GameContext& context);
};

// シーンマネージャー
class SceneManager
{
private:
	// シーンタスク
	struct SceneTask
	{
		std::wstring name;
		LoadSceneMode mode;
	};

	std::unordered_map<std::wstring, std::unique_ptr<ISceneBuilder>> m_sceneBuilders;
	std::queue<SceneTask> m_loadQueue;
	SceneView m_sceneView;

public:
	SceneManager();

public:
	inline const SceneInfo& GetActiveScene() const
	{
		return *m_sceneView.scenes.front();
	}

	inline SceneView& GetSceneView()
	{
		return m_sceneView;
	}

	void Register(std::unique_ptr<ISceneBuilder>&& sceneBuilder);

	template<typename T, typename... Args>
	void Register(Args... args)
	{
		Register(std::make_unique<T>(args...));
	}

	void LoadScene(const std::wstring& name, LoadSceneMode mode = LoadSceneMode::Single);
	bool IsSceneValid(const std::wstring& name);
	void ProcessScene(GameContext& context);
};
