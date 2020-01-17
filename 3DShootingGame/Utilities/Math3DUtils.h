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
