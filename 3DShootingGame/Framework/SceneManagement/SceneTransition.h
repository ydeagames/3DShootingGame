#pragma once
#include <Framework/ECS/Component.h>

// �V�[���J��
class SceneTransition : public Component
{
public:
	std::function<void(GameContext&)> unloadBefore;
	std::function<void(GameContext&)> loadAfter;
};
