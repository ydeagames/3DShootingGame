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
