#pragma once
#include "SceneTransition.h"

namespace SceneTransitions
{
	std::shared_ptr<SceneTransition> CreateFadeTransition(float duration);
	std::shared_ptr<SceneTransition> CreateRuleTransition(float duration);
}
