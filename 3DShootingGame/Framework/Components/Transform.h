#pragma once
#include <Framework/ECS/Component.h>

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

class TransformResolver
{
private:
	entt::SparseSet<entt::entity, DirectX::SimpleMath::Matrix> matrixMap;

public:
	void ClearCache();
	DirectX::SimpleMath::Matrix Resolve(const GameObject& transform);
};

// Transformクラス
class Transform : public Component
{
public:
	// コンストラクタ
	Transform::Transform();

public:
	// ローカル位置
	DirectX::SimpleMath::Vector3 localPosition;
	// ローカル回転
	DirectX::SimpleMath::Quaternion localRotation;
	// ローカルスケール
	DirectX::SimpleMath::Vector3 localScale;

	DirectX::SimpleMath::Vector3 GetPosition() const;
	void SetPosition(const DirectX::SimpleMath::Vector3& value);
	__declspec(property(get = GetPosition, put = SetPosition)) DirectX::SimpleMath::Vector3 position;

	DirectX::SimpleMath::Quaternion GetRotation() const;
	void SetRotation(const DirectX::SimpleMath::Quaternion& value);
	__declspec(property(get = GetRotation, put = SetRotation)) DirectX::SimpleMath::Quaternion rotation;

	DirectX::SimpleMath::Vector3 GetLossyScale() const;
	void SetLossyScale(const DirectX::SimpleMath::Vector3& value);
	__declspec(property(get = GetLossyScale, put = SetLossyScale)) DirectX::SimpleMath::Vector3 lossyScale;

	const Transform* parent;

	void SetParent(const Transform* value);

	bool IsParentDestroyed() const;

	// 行列取得
	DirectX::SimpleMath::Matrix GetMatrix() const;
};