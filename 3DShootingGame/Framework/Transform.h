#pragma once
#include "Component.h"

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