#pragma once
#include "Components.h"
#include "Background.h"

using Components = std::tuple<
	MoveUpdater,
	MoveDownUpdater,
	PrimitiveRenderer,
	UpdateRenderer,
	CameraComponent,
	Background
>;