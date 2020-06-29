// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-09 12:25:31 +0900
// Modified: 2020-02-07 10:59:30 +0900

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
#include "ShaderArt.h"
#include "BubbleEffect.h"
#include "ShootGauge.h"
#include "AudioSource.h"
#include "LightComponent.h"

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
		PauseMenu,
		FPSCamera,
		InfinityGridFloor,
		FontRenderer,
		PlayerController,
		TranslateFollower,
		RotateFollower,
		TrackingFollower,
		RainbowEffect,
		Goal,
		ShaderArt,
		BubbleEffect,
		ShootGauge,
		AudioSource,
		Death,
		LightComponent
	>;
}


