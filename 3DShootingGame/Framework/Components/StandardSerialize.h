#pragma once

// シリアライズ

namespace DirectX
{
	// SimpleMathシリアライズ
	namespace SimpleMath
	{
		// Vector4: x, y, z, w
		template<typename Archive>
		void serialize(Archive& archive, Vector4& vector) {
			archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y), cereal::make_nvp("z", vector.z), cereal::make_nvp("w", vector.w));
		}

		// Vector3: x, y, z
		template<typename Archive>
		void serialize(Archive& archive, Vector3& vector) {
			archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y), cereal::make_nvp("z", vector.z));
		}

		// Vector2: x, y
		template<typename Archive>
		void serialize(Archive& archive, Vector2& vector) {
			archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y));
		}

		// Quaternion: x, y, z, w
		template<typename Archive>
		void serialize(Archive& archive, Quaternion& quat) {
			archive(cereal::make_nvp("x", quat.x), cereal::make_nvp("y", quat.y), cereal::make_nvp("z", quat.z), cereal::make_nvp("w", quat.w));
		}

		// Color: x, y, z, w
		template<typename Archive>
		void serialize(Archive& archive, Color& color) {
			archive(cereal::make_nvp("x", color.x), cereal::make_nvp("y", color.y), cereal::make_nvp("z", color.z), cereal::make_nvp("w", color.w));
		}
	}
}

// PhysXシリアライズ
namespace physx
{
	// PxRigidBodyFlags: flags
	template<typename Archive>
	void serialize(Archive& archive, PxRigidBodyFlags& flags) {
		uint32_t flag = flags;
		archive(cereal::make_nvp("flags", flag));
		flags = PxRigidBodyFlags(PxU8(flag));
	}
}
