#pragma once

class Light
{
public:
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 direction;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;

public:
	Light();
	
public:
	void SetLookAt(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& target, const DirectX::SimpleMath::Vector3& up);
	void SetProjection(const DirectX::SimpleMath::Matrix& proj);
};
