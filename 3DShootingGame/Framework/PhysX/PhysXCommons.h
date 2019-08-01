#pragma once

template<typename T>
inline void px_release(T* t)
{
	if (t)
		t->release();
}

template<typename T>
struct px_deleter
{
	void operator()(T* t)
	{
		px_release(t);
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

struct PhysicsMaterial
{
	float dynamicFriction;
	float staticFriction;
	float bounciness;
	physx::PxCombineMode::Enum frictionCombine;
	physx::PxCombineMode::Enum bounceCombine;
};

namespace PhysicsMaterials
{
	constexpr const PhysicsMaterial Bouncy			= { .3f, .3f, 1.f, physx::PxCombineMode::eAVERAGE, physx::PxCombineMode::eMAX };
	constexpr const PhysicsMaterial Ice				= { .1f, .1f, 0.f, physx::PxCombineMode::eMULTIPLY, physx::PxCombineMode::eMULTIPLY };
	constexpr const PhysicsMaterial MaxFriction		= { 1.f, 1.f, 0.f, physx::PxCombineMode::eMAX, physx::PxCombineMode::eAVERAGE };
	constexpr const PhysicsMaterial Metal			= { .15f, .15f, 1.f, physx::PxCombineMode::eMIN, physx::PxCombineMode::eAVERAGE };
	constexpr const PhysicsMaterial Rubber			= { 1.f, 1.f, .5f, physx::PxCombineMode::eMAX, physx::PxCombineMode::eAVERAGE };
	constexpr const PhysicsMaterial Wood			= { .45f, .45f, 0.f, physx::PxCombineMode::eAVERAGE, physx::PxCombineMode::eAVERAGE };
	constexpr const PhysicsMaterial ZeroFriction	= { 0.f, 0.f, 0.f, physx::PxCombineMode::eMULTIPLY, physx::PxCombineMode::eAVERAGE };
}

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