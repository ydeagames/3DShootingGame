// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-02-05 23:03:19 +0900
// Modified: 2020-02-05 23:03:19 +0900

#include "pch.h"
#include "LightComponent.h"
#include <Framework/Context/GameCamera.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Transform.h>
#include "Framework/Shadow/Light.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void LightComponent::OnPreRender(GameCamera& camera)
{
	Vector3 s, t;
	Quaternion r;
	gameObject.GetComponent<Transform>().GetMatrix().Decompose(s, r, t);
	GameContext::Get<Light>().SetLook(t, Vector3::Transform(Vector3::Forward, r), Vector3::Up);
}


