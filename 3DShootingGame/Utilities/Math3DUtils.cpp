#include "pch.h"
#include "Math3DUtils.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace Math3DUtils
{
	DirectX::SimpleMath::Quaternion ToQuaternion(const DirectX::SimpleMath::Vector3& euler) // yaw (Z), pitch (Y), roll (X)
	{
		// Abbreviations for the various angular functions
		float cy = std::cos(euler.z * 0.5f);
		float sy = std::sin(euler.z * 0.5f);
		float cp = std::cos(euler.y * 0.5f);
		float sp = std::sin(euler.y * 0.5f);
		float cr = std::cos(euler.x * 0.5f);
		float sr = std::sin(euler.x * 0.5f);

		Quaternion q;
		q.w = cy * cp * cr + sy * sp * sr;
		q.x = cy * cp * sr - sy * sp * cr;
		q.y = sy * cp * sr + cy * sp * cr;
		q.z = sy * cp * cr - cy * sp * sr;

		return q;
	}

	DirectX::SimpleMath::Vector3 ToEulerAngles(const DirectX::SimpleMath::Quaternion& q) {
		Vector3 angles;

		// roll (x-axis rotation)
		float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		angles.x = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		float sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
			angles.y = std::copysign(XM_PI / 2, sinp); // use 90 degrees if out of range
		else
			angles.y = std::asin(sinp);

		// yaw (z-axis rotation)
		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.z = std::atan2(siny_cosp, cosy_cosp);

		return angles;
	}

	DirectX::SimpleMath::Quaternion LookAt(const DirectX::SimpleMath::Vector3& delta)
	{
		auto z = delta;
		z.Normalize();
		auto x = Vector3::Up.Cross(z);
		x.Normalize();
		auto y = z.Cross(x);
		y.Normalize();

		Matrix m = Matrix::Identity;
		m(0, 0) = x.x; m(0, 1) = y.x; m(0, 2) = z.x;
		m(1, 0) = x.y; m(1, 1) = y.y; m(1, 2) = z.y;
		m(2, 0) = x.z; m(2, 1) = y.z; m(2, 2) = z.z;

		Vector3 s, t;
		Quaternion r;
		m.Decompose(s, r, t);

		return r;
	}

	DirectX::SimpleMath::Quaternion LookAt(const DirectX::SimpleMath::Vector3& from, const DirectX::SimpleMath::Vector3& to)
	{
		return LookAt(to - from);
	}
}
