#pragma once
#include <Framework/ECS/Component.h>

// ƒV[ƒ“‘JˆÚ
class SceneTransition : public Component
{
public:
	std::function<void()> unloadBefore;
	std::function<void()> loadAfter;
};
