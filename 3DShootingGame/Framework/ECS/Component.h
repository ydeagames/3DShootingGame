#pragma once
#include "GameObject.h"
#include "ComponentManager.h"

class Transform;

class TransformResolver
{
private:
	entt::SparseSet<entt::entity, DirectX::SimpleMath::Matrix> matrixMap;

public:
	void ClearCache();
	DirectX::SimpleMath::Matrix Resolve(const GameObject& transform);
};

class Component
{
public:
	static constexpr const char* Identifier = "Component";
	GameObject gameObject;

	//public:
	//	Component() = default;
	//	~Component() = default;
	//	Component(const Component& c) : gameObject(c.gameObject) { std::cout << "Copy Constructor" << std::endl; }
	//	Component& operator=(const Component& c) { std::cout << "Copy Assignment" << std::endl; gameObject = c.gameObject; return *this; }
};

class Transform : public Component
{
public:
	static constexpr const char* Identifier = "Transform";

public:
	std::string name;
	entt::entity parent = entt::null;
	//bool isStatic = false;

public:
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 scale = { 1, 1, 1 };

public:
	DirectX::SimpleMath::Matrix GetLocalMatrix() const;
	DirectX::SimpleMath::Matrix GetMatrix() const;

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(name), CEREAL_OPTIONAL_NVP(parent)/*, CEREAL_OPTIONAL_NVP(isStatic)*/);
		archive(CEREAL_OPTIONAL_NVP(position), CEREAL_OPTIONAL_NVP(rotation), CEREAL_OPTIONAL_NVP(scale));
	}

	template<class Reference>
	void Reference(Reference& reference)
	{
		reference(parent);
	}

	void EditorGui();
};

