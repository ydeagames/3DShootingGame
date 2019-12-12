#pragma once
#include "Components.h"
#include "Background.h"
#include "Menu.h"
#include "FPSCamera.h"
#include "PrimitiveRenderer.h"
#include "ModelRenderer.h"
#include "InfinityGridFloor.h"

using Components = std::tuple<
	MoveUpdater,
	MoveDownUpdater,
	PrimitiveRenderer,
	ModelRenderer,
	UpdateRenderer,
	CameraComponent,
	Background,
	TitleMenu,
	PlayMenu,
	FPSCamera,
	InfinityGridFloor
>;