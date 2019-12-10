#pragma once
#include <Framework/ECS/GameContext.h>
#include <Framework/ECS/GameObject.h>
#include <Framework/ECS/Events.h>
#include <Common/DeviceResources.h>
#include <Framework/ECS/Component.h>
#include <Framework/Components/StandardSerialize.h>

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
	void Render(GameCamera& camera);

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

protected:
	GameCamera* cameraptr = nullptr;

public:
	void Render(GameCamera& camera);
	void Update();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};