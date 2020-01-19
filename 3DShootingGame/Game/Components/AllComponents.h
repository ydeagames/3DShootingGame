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
#include "AudioSource.h"

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
		AudioSource
	>;
}
