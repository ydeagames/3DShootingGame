#pragma once
#include "CameraComponent.h"
#include "Background.h"
#include "Menu.h"
#include "FPSCamera.h"
#include "PrimitiveRenderer.h"
#include "ModelRenderer.h"
#include "InfinityGridFloor.h"

namespace BuildSettings
{
	using Components = std::tuple<
		PrimitiveRenderer,
		ModelRenderer,
		CameraComponent,
		Background,
		TitleMenu,
		PlayMenu,
		FPSCamera,
		InfinityGridFloor
	>;
}