#pragma once
#include "SceneTransition.h"

namespace SceneTransitions
{
	std::shared_ptr<SceneTransition> CreateFadeTransition(float duration);
}
