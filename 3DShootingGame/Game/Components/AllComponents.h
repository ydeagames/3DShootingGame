#pragma once
#include "Components.h"
#include "Background.h"
#include "Menu.h"
#include "FPSCamera.h"

using Components = std::tuple<
	MoveUpdater,
	MoveDownUpdater,
	PrimitiveRenderer,
	UpdateRenderer,
	CameraComponent,
	Background,
	TitleMenu,
	PlayMenu,
	FPSCamera
>;