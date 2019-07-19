#pragma once

template<typename T>
struct px_deleter
{
	void operator()(T* t)
	{
		if (t)
			t->release();
	}
};

template<typename T>
using px_unique_ptr = std::unique_ptr<T, px_deleter<T>>;

template<typename T>
struct px_shared_ptr : public std::shared_ptr<T>
{
	px_shared_ptr(T* _Px)
		: std::shared_ptr(_Px, px_deleter<T>)
	{
	}
};

namespace physx
{
	inline DirectX::SimpleMath::Vector2 fromPhysX(const physx::PxVec2& vec) { return DirectX::SimpleMath::Vector2(vec.x, vec.y); }
	inline DirectX::SimpleMath::Vector3 fromPhysX(const physx::PxVec3& vec) { return DirectX::SimpleMath::Vector3(vec.x, vec.y, vec.z); }
	inline DirectX::SimpleMath::Vector4 fromPhysX(const physx::PxVec4& vec) { return DirectX::SimpleMath::Vector4(vec.x, vec.y, vec.z, vec.w); }
	inline DirectX::SimpleMath::Quaternion fromPhysX(const physx::PxQuat& quat) { return DirectX::SimpleMath::Quaternion(quat.x, quat.y, quat.z, quat.w); }
	inline DirectX::SimpleMath::Matrix fromPhysX(const physx::PxMat44& mat) { return DirectX::SimpleMath::Matrix(&mat.column0.x); }

	inline physx::PxVec2 toPhysX(const DirectX::SimpleMath::Vector2& vec) { return physx::PxVec2(vec.x, vec.y); }
	inline physx::PxVec3 toPhysX(const DirectX::SimpleMath::Vector3& vec) { return physx::PxVec3(vec.x, vec.y, vec.z); }
	inline physx::PxVec4 toPhysX(const DirectX::SimpleMath::Vector4& vec) { return physx::PxVec4(vec.x, vec.y, vec.z, vec.w); }
	inline physx::PxQuat toPhysX(const DirectX::SimpleMath::Quaternion& quat) { return physx::PxQuat(quat.x, quat.y, quat.z, quat.w); }
	inline physx::PxMat44 toPhysX(const DirectX::SimpleMath::Matrix& mat) { return physx::PxMat44(physx::PxVec4(mat.m[0]), physx::PxVec4(mat.m[1]), physx::PxVec4(mat.m[2]), physx::PxVec4(mat.m[3])); }
}