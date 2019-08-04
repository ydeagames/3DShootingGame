#pragma once
#include <Framework/Component.h>

// ƒV[ƒ“‘JˆÚ
class SceneTransition : public Component
{
public:
	std::function<void(GameContext&)> unloadBefore;
	std::function<void(GameContext&)> loadAfter;
};
