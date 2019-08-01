#pragma once
#include "Component.h"
#include <Utilities/Property.h>

// Transformクラス
class Transform : public Component
{
public:
	// コンストラクタ
	Transform::Transform()
		: parent(nullptr)
		, localPosition(DirectX::SimpleMath::Vector3::Zero)
		, localRotation(DirectX::SimpleMath::Quaternion::Identity)
		, localScale(DirectX::SimpleMath::Vector3::One)
	{
	}

public:
	// ローカル位置
	DirectX::SimpleMath::Vector3 localPosition;
	// ローカル回転
	DirectX::SimpleMath::Quaternion localRotation;
	// ローカルスケール
	DirectX::SimpleMath::Vector3 localScale;

	DirectX::SimpleMath::Vector3 GetPosition() const
	{
		if (parent)
			return DirectX::SimpleMath::Vector3::Transform(localPosition, parent->GetMatrix());
		else
			return localPosition;
	}
	void SetPosition(const DirectX::SimpleMath::Vector3& value)
	{
		if (parent)
			localPosition = DirectX::SimpleMath::Vector3::Transform(value, parent->GetMatrix().Invert());
		else
			localPosition = value;
	}
	__declspec(property(get = GetPosition, put = SetPosition)) DirectX::SimpleMath::Vector3 position;

	DirectX::SimpleMath::Quaternion GetRotation() const
	{
		if (parent)
			return localRotation * DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(parent->GetMatrix());
		else
			return localRotation;
	}
	void SetRotation(const DirectX::SimpleMath::Quaternion& value)
	{
		if (parent)
			localRotation = value * DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(parent->GetMatrix().Invert());
		else
			localRotation = value;
	};
	__declspec(property(get = GetRotation, put = SetRotation)) DirectX::SimpleMath::Quaternion rotation;

	DirectX::SimpleMath::Vector3 GetLossyScale() const
	{
		if (parent)
		{
			DirectX::SimpleMath::Vector3 parentPosition;
			DirectX::SimpleMath::Quaternion parentRotation;
			DirectX::SimpleMath::Vector3 parentScale;
			parent->GetMatrix().Decompose(parentScale, parentRotation, parentPosition);
			auto mat = DirectX::SimpleMath::Matrix::CreateScale(parentScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(parentRotation);
			return DirectX::SimpleMath::Vector3::Transform(localScale, mat);
		}
		else
			return localScale;
	}
	void SetLossyScale(const DirectX::SimpleMath::Vector3& value)
	{
		if (parent)
		{
			DirectX::SimpleMath::Vector3 parentPosition;
			DirectX::SimpleMath::Quaternion parentRotation;
			DirectX::SimpleMath::Vector3 parentScale;
			parent->GetMatrix().Decompose(parentScale, parentRotation, parentPosition);
			auto mat = DirectX::SimpleMath::Matrix::CreateScale(parentScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(parentRotation);
			localScale = DirectX::SimpleMath::Vector3::Transform(value, mat.Invert());
		}
		else
			localScale = value;
	}
	__declspec(property(get = GetLossyScale, put = SetLossyScale)) DirectX::SimpleMath::Vector3 lossyScale;

	const Transform* parent;
	void SetParent(const Transform* value)
	{
		DirectX::SimpleMath::Vector3 worldPosition = position;
		DirectX::SimpleMath::Quaternion worldRotation = rotation;
		DirectX::SimpleMath::Vector3 worldScale = lossyScale;
		parent = value;
		position = worldPosition;
		rotation = worldRotation;
		lossyScale = worldScale;
	}

	// 行列取得
	DirectX::SimpleMath::Matrix GetMatrix() const
	{
		return
			DirectX::SimpleMath::Matrix::CreateScale(localScale) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(localRotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(localPosition) *
			(parent != nullptr ? parent->GetMatrix() : DirectX::SimpleMath::Matrix::Identity);
	}
};