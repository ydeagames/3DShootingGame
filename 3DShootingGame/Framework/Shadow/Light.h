// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-02-05 23:03:19 +0900
// Modified: 2020-02-05 23:03:19 +0900

#pragma once

class Light
{
public:
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 direction;
	DirectX::SimpleMath::Vector3 up;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;

public:
	Light();
	
public:
	void SetLook(const DirectX::SimpleMath::Vector3& positionVec, const DirectX::SimpleMath::Vector3& directionVec, const DirectX::SimpleMath::Vector3& upVec);
	void SetPosition(const DirectX::SimpleMath::Vector3& positionVec);
	void SetProjection(const DirectX::SimpleMath::Matrix& proj);
};


