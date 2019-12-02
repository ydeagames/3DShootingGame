#pragma once
#include <Framework/ECS/Scene.h>

class SceneManager
{
private:
	bool requested;
	SceneInfo* nextScene;

public:
	void Apply();
	void LoadScene(const SceneInfo& info);
};
