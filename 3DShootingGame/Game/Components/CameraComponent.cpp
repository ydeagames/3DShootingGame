// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-16 06:22:52 +0900

#include "pch.h"
#include "CameraComponent.h"
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void CameraComponent::OnPreRender(GameCamera& camera)
{
	Vector3 s, t;
	Quaternion r;
	gameObject.GetComponent<Transform>().GetMatrix().Decompose(s, r, t);
	camera.view = Matrix::CreateScale(Vector3::One / s) * DirectX::SimpleMath::Matrix::CreateLookAt(t,
		t + Vector3::Transform(Vector3::Forward, Matrix::CreateFromQuaternion(r)),
		Vector3::Transform(Vector3::Up, Matrix::CreateFromQuaternion(r)));
}


