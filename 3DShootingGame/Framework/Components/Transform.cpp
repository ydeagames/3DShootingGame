#include "pch.h"
#include "Transform.h"
#include <Framework/ECS/GameContext.h>
#include <Utilities/Math3DUtils.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

Transform::Transform()
	: localPosition(Vector3::Zero)
	, localRotation(Quaternion::Identity)
	, localScale(Vector3::One)
{
}

DirectX::SimpleMath::Vector3 Transform::GetPosition() const
{
	auto p = gameObject.GetParent();
	if (p != entt::null)
		return Vector3::Transform(localPosition, GameContext::Get<TransformResolver>().Resolve(gameObject.Wrap(p)));
	else
		return localPosition;
}

void Transform::SetPosition(const DirectX::SimpleMath::Vector3& value)
{
	auto p = gameObject.GetParent();
	if (p != entt::null)
		localPosition = Vector3::Transform(value, GameContext::Get<TransformResolver>().Resolve(gameObject.Wrap(p)).Invert());
	else
		localPosition = value;
}

DirectX::SimpleMath::Quaternion Transform::GetRotation() const
{
	auto p = gameObject.GetParent();
	if (p != entt::null)
		return localRotation * Quaternion::CreateFromRotationMatrix(GameContext::Get<TransformResolver>().Resolve(gameObject.Wrap(p)));
	else
		return localRotation;
}

void Transform::SetRotation(const DirectX::SimpleMath::Quaternion& value)
{
	auto p = gameObject.GetParent();
	if (p != entt::null)
		localRotation = value * Quaternion::CreateFromRotationMatrix(GameContext::Get<TransformResolver>().Resolve(gameObject.Wrap(p)).Invert());
	else
		localRotation = value;
}

DirectX::SimpleMath::Vector3 Transform::GetLossyScale() const
{
	auto p = gameObject.GetParent();
	if (p != entt::null)
	{
		Vector3 parentPosition;
		Quaternion parentRotation;
		Vector3 parentScale;
		GameContext::Get<TransformResolver>().Resolve(gameObject.Wrap(p)).Decompose(parentScale, parentRotation, parentPosition);
		auto mat = Matrix::CreateScale(parentScale) * Matrix::CreateFromQuaternion(parentRotation);
		return Vector3::Transform(localScale, mat);
	}
	else
		return localScale;
}

void Transform::SetLossyScale(const DirectX::SimpleMath::Vector3& value)
{
	auto p = gameObject.GetParent();
	if (p != entt::null)
	{
		Vector3 parentPosition;
		Quaternion parentRotation;
		Vector3 parentScale;
		GameContext::Get<TransformResolver>().Resolve(gameObject.Wrap(p)).Decompose(parentScale, parentRotation, parentPosition);
		auto mat = Matrix::CreateScale(parentScale) * Matrix::CreateFromQuaternion(parentRotation);
		localScale = Vector3::Transform(value, mat.Invert());
	}
	else
		localScale = value;
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
		auto euler = Math3DUtils::ToEulerAngles(DirectX::SimpleMath::Quaternion(t.localRotation.x, t.localRotation.y, t.localRotation.z, t.localRotation.w)) * (180.f / DirectX::XM_PI);

		float rot[] = { euler.x, euler.y, euler.z };

		// the "##Transform" ensures that you can use the name "x" in multiple lables
		ImGui::DragFloat3("Rotation##Transform", &rot[0], 0.1f);

		auto quat = Math3DUtils::ToQuaternion(DirectX::SimpleMath::Vector3(rot[0], rot[1], rot[2]) * (DirectX::XM_PI / 180.f));
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
