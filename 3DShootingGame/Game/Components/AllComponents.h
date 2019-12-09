#pragma once
#include "Components.h"

using Components = std::tuple<
	MoveUpdater,
	MoveDownUpdater,
	PrimitiveRenderer,
	UpdateRenderer,
	CameraComponent
>;