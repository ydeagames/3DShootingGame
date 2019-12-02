#pragma once
#include <Framework/ECS/GameContext.h>
#include <Framework/ECS/GameObject.h>
#include <Framework/ECS/Events.h>
#include <Common/DeviceResources.h>
#include <Framework/ECS/Component.h>

namespace DirectX
{
	namespace SimpleMath
	{
		template<typename Archive>
		void serialize(Archive& archive, Vector3& vector) {
			archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y), cereal::make_nvp("z", vector.z));
		}

		template<typename Archive>
		void serialize(Archive& archive, Quaternion& quat) {
			archive(cereal::make_nvp("x", quat.x), cereal::make_nvp("y", quat.y), cereal::make_nvp("z", quat.z), cereal::make_nvp("w", quat.w));
		}
	}
}

class MoveUpdater : public Component
{
public:
	static constexpr const char* Identifier = "MoveUpdater";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	DirectX::SimpleMath::Vector3 vel;

public:
	void Start();
	void Update();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class MoveDownUpdater : public Component
{
public:
	static constexpr const char* Identifier = "MoveDownUpdater";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	void Update();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class PrimitiveRenderer : public Component
{
public:
	static constexpr const char* Identifier = "PrimitiveRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;

public:
	void Awake() { std::cout << "Awake" << gameObject.entity << std::endl; }
	void OnDestroy() { std::cout << "OnDestroy" << gameObject.entity << std::endl; }
	void RenderStart();
	void Render(Camera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class UpdateRenderer : public MoveUpdater
{
public:
	static constexpr const char* Identifier = "UpdateRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<MoveUpdater, Transform, PrimitiveRenderer>();
	}

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class CameraComponent : public Component
{
public:
	static constexpr const char* Identifier = "CameraComponent";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

private:
	Camera* cameraptr = nullptr;

public:
	void Render(Camera& camera);
	void Update();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};