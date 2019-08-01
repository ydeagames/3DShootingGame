#include "pch.h"
#include "Transform.h"
#include "GameObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Transform::Transform()
	: parent(nullptr)
	, localPosition(Vector3::Zero)
	, localRotation(Quaternion::Identity)
	, localScale(Vector3::One)
{
}

DirectX::SimpleMath::Vector3 Transform::GetPosition() const
{
	if (parent)
		return Vector3::Transform(localPosition, parent->GetMatrix());
	else
		return localPosition;
}

void Transform::SetPosition(const DirectX::SimpleMath::Vector3& value)
{
	if (parent)
		localPosition = Vector3::Transform(value, parent->GetMatrix().Invert());
	else
		localPosition = value;
}

DirectX::SimpleMath::Quaternion Transform::GetRotation() const
{
	if (parent)
		return localRotation * Quaternion::CreateFromRotationMatrix(parent->GetMatrix());
	else
		return localRotation;
}

void Transform::SetRotation(const DirectX::SimpleMath::Quaternion& value)
{
	if (parent)
		localRotation = value * Quaternion::CreateFromRotationMatrix(parent->GetMatrix().Invert());
	else
		localRotation = value;
}

DirectX::SimpleMath::Vector3 Transform::GetLossyScale() const
{
	if (parent)
	{
		Vector3 parentPosition;
		Quaternion parentRotation;
		Vector3 parentScale;
		parent->GetMatrix().Decompose(parentScale, parentRotation, parentPosition);
		auto mat = Matrix::CreateScale(parentScale) * Matrix::CreateFromQuaternion(parentRotation);
		return Vector3::Transform(localScale, mat);
	}
	else
		return localScale;
}

void Transform::SetLossyScale(const DirectX::SimpleMath::Vector3& value)
{
	if (parent)
	{
		Vector3 parentPosition;
		Quaternion parentRotation;
		Vector3 parentScale;
		parent->GetMatrix().Decompose(parentScale, parentRotation, parentPosition);
		auto mat = Matrix::CreateScale(parentScale) * Matrix::CreateFromQuaternion(parentRotation);
		localScale = Vector3::Transform(value, mat.Invert());
	}
	else
		localScale = value;
}

void Transform::SetParent(const Transform* value)
{
	Vector3 worldPosition = position;
	Quaternion worldRotation = rotation;
	Vector3 worldScale = lossyScale;
	parent = value;
	position = worldPosition;
	rotation = worldRotation;
	lossyScale = worldScale;
}

bool Transform::IsParentDestroyed() const
{
	if (gameObject && gameObject->IsDestroyed())
		return true;
	if (parent && parent->IsParentDestroyed())
		return true;
	return Object::IsDestroyed();
}

DirectX::SimpleMath::Matrix Transform::GetMatrix() const
{
	return
		Matrix::CreateScale(localScale) *
		Matrix::CreateFromQuaternion(localRotation) *
		Matrix::CreateTranslation(localPosition) *
		(parent != nullptr ? parent->GetMatrix() : Matrix::Identity);
}
