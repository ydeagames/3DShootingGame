#pragma once
#include <Framework/ECS/Component.h>

// �V�[���J��
class SceneTransition : public Component
{
public:
	std::function<void()> unloadBefore;
	std::function<void()> loadAfter;
};
