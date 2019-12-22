#pragma once
#include "CameraComponent.h"
#include "Background.h"
#include "Menu.h"
#include "FPSCamera.h"
#include "PrimitiveRenderer.h"
#include "ModelRenderer.h"
#include "InfinityGridFloor.h"
#include "FontRenderer.h"
#include "PlayerController.h"
#include "PlayerCamera.h"
#include "RainbowEffect.h"
#include "Goal.h"

namespace BuildSettings
{
	using Components = std::tuple<
		PrimitiveRenderer,
		ModelRenderer,
		CameraComponent,
		Background,
		TitleMenu,
		PlayMenu,
		ResultMenu,
		FPSCamera,
		InfinityGridFloor,
		FontRenderer,
		PlayerController,
		TranslateFollower,
		RotateFollower,
		TrackingFollower,
		RainbowEffect,
		Goal
	>;
}