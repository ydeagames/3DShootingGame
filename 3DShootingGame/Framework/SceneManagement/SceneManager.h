#pragma once
#include <Framework/ISceneBuilder.h>

// �V�[���I�v�V����
enum class LoadSceneMode
{
	Single,
	Additive,
};

// �V�[���J��
class SceneTransition : public Component
{
public:
	std::function<void(GameContext&)> unloadBefore;
	std::function<void(GameContext&)> loadAfter;
};

namespace SceneTransitions
{
	std::shared_ptr<SceneTransition> CreateFadeTransition(float duration);
}

// �V�[���r���[
struct SceneView : public Component
{
public:
	std::deque<std::unique_ptr<Scene>> scenes;
	std::shared_ptr<SceneTransition> transition;

public:
	// ����
	void Initialize(GameContext& context);
	// �X�V
	void Update(GameContext& context);
	// �`��
	void Render(GameContext& context);
	// �j��
	void Finalize(GameContext& context);
};

// �V�[���}�l�[�W���[
class SceneManager
{
private:
	// �V�[���^�X�N
	struct SceneTask
	{
		std::wstring name;
		LoadSceneMode mode;
	};

	std::unordered_map<std::wstring, std::unique_ptr<ISceneBuilder>> m_sceneBuilders;
	std::queue<SceneTask> m_loadQueue;
	SceneView m_sceneView;
	std::shared_ptr<SceneTransition> m_transitionQueue;

public:
	SceneManager();

public:
	inline Scene& GetActiveScene() const
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

	void LoadSceneWithTransition(const std::wstring& name, const std::shared_ptr<SceneTransition>& transition = SceneTransitions::CreateFadeTransition(2.f));
};