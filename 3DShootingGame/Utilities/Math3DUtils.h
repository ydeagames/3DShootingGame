// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-03 17:52:30 +0900
// Modified: 2020-01-17 10:45:52 +0900

#pragma once

namespace Math3DUtils
{
	DirectX::SimpleMath::Quaternion ToQuaternion(const DirectX::SimpleMath::Vector3& euler); // yaw (Z), pitch (Y), roll (X)
	DirectX::SimpleMath::Vector3 ToEulerAngles(const DirectX::SimpleMath::Quaternion& q);
	DirectX::SimpleMath::Quaternion LookAt(const DirectX::SimpleMath::Vector3& from, const DirectX::SimpleMath::Vector3& to);
	float Angle(const DirectX::SimpleMath::Vector3& from, const DirectX::SimpleMath::Vector3& to);
	DirectX::SimpleMath::Quaternion FromToRotation(const DirectX::SimpleMath::Vector3& from, const DirectX::SimpleMath::Vector3& to);
	DirectX::SimpleMath::Vector3 Normalized(const DirectX::SimpleMath::Vector3& vec);
}


