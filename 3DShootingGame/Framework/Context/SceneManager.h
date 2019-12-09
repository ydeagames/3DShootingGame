#pragma once
#include <Framework/ECS/Scene.h>

class SceneManager
{
private:
	Scene* scene;
	bool requested = false;
	SceneInfo nextScene;

public:
	SceneManager(Scene& scene);

public:
	Scene& GetActiveScene();
	void Apply();
	void LoadScene(const SceneInfo& info);
};
