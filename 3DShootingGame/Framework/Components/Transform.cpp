#include "pch.h"
#include "Transform.h"
#include <Framework/ECS/GameContext.h>

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
	Vector3 worldPosition = localPosition;
	Quaternion worldRotation = localRotation;
	Vector3 worldScale = lossyScale;
	parent = value;
	localPosition = worldPosition;
	localRotation = worldRotation;
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

DirectX::SimpleMath::Matrix Transform::GetLocalMatrix() const
{
	return Matrix::CreateScale(localScale)
		* Matrix::CreateFromQuaternion(localRotation)
		* Matrix::CreateTranslation(localPosition);
}

DirectX::SimpleMath::Matrix Transform::GetMatrix() const
{
	return GameContext::Get<TransformResolver>().Resolve(gameObject);
}

void Transform::EditorGui()
{
	auto& t = *this;
	auto& reg = *gameObject.registry;

	std::string tmpname = t.name;
	tmpname.resize(16);
	ImGui::InputText("Name##Transform", &tmpname[0], tmpname.size());
	t.name = std::string(tmpname.c_str());

	{
		auto& e = t.parent;
		int iid = (e == entt::null) ? -1 : int(reg.entity(e));
		ImGui::InputInt("Parent##Transform", &iid);

		if (iid < 0)
			e = entt::null;
		else
		{
			auto id = entt::entity(iid);
			e = id < reg.size() ? (id | reg.current(id) << entt::entt_traits<entt::entity>::entity_shift) : id;
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_Hierarchy"))
				e = *(static_cast<const entt::entity*>(payload->Data));
			ImGui::EndDragDropTarget();
		}
	}

	// the "##Transform" ensures that you can use the name "x" in multiple lables
	ImGui::DragFloat3("Position##Transform", &t.localPosition.x, 0.1f);

	{
		auto euler = ToEulerAngles(DirectX::SimpleMath::Quaternion(t.localRotation.x, t.localRotation.y, t.localRotation.z, t.localRotation.w)) * (180.f / DirectX::XM_PI);

		float rot[] = { euler.x, euler.y, euler.z };

		// the "##Transform" ensures that you can use the name "x" in multiple lables
		ImGui::DragFloat3("Rotation##Transform", &rot[0], 0.1f);

		auto quat = ToQuaternion(DirectX::SimpleMath::Vector3(rot[0], rot[1], rot[2]) * (DirectX::XM_PI / 180.f));
		t.localRotation.x = quat.x;
		t.localRotation.y = quat.y;
		t.localRotation.z = quat.z;
		t.localRotation.w = quat.w;
	}

	// the "##Transform" ensures that you can use the name "x" in multiple lables
	ImGui::DragFloat3("Scale##Transform", &t.localScale.x, 0.1f);
}

void TransformResolver::ClearCache()
{
	matrixMap.reset();
}

DirectX::SimpleMath::Matrix TransformResolver::Resolve(const GameObject& gameObject)
{
	if (matrixMap.has(gameObject.entity))
		return matrixMap.get(gameObject.entity);
	if (gameObject.HasComponent<Transform>())
	{
		auto& transform = gameObject.GetComponent<Transform>();
		auto matrix = transform.GetLocalMatrix();
		if (gameObject.registry->valid(transform.parent))
			matrix *= Resolve(gameObject.Wrap(transform.parent));
		matrixMap.construct(gameObject.entity, matrix);
		return matrix;
	}
	else
	{
		auto matrix = DirectX::SimpleMath::Matrix::Identity;
		matrixMap.construct(gameObject.entity, matrix);
		return matrix;
	}
}
