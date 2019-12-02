#include "pch.h"
#include "Component.h"
#include "GameContext.h"

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

namespace
{
	DirectX::SimpleMath::Quaternion ToQuaternion(const DirectX::SimpleMath::Vector3& euler) // yaw (Z), pitch (Y), roll (X)
	{
		// Abbreviations for the various angular functions
		float cy = std::cos(euler.z * 0.5f);
		float sy = std::sin(euler.z * 0.5f);
		float cp = std::cos(euler.y * 0.5f);
		float sp = std::sin(euler.y * 0.5f);
		float cr = std::cos(euler.x * 0.5f);
		float sr = std::sin(euler.x * 0.5f);

		DirectX::SimpleMath::Quaternion q;
		q.w = cy * cp * cr + sy * sp * sr;
		q.x = cy * cp * sr - sy * sp * cr;
		q.y = sy * cp * sr + cy * sp * cr;
		q.z = sy * cp * cr - cy * sp * sr;

		return q;
	}

	DirectX::SimpleMath::Vector3 ToEulerAngles(const DirectX::SimpleMath::Quaternion& q) {
		DirectX::SimpleMath::Vector3 angles;

		// roll (x-axis rotation)
		float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		angles.x = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		float sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
			angles.y = std::copysign(DirectX::XM_PI / 2, sinp); // use 90 degrees if out of range
		else
			angles.y = std::asin(sinp);

		// yaw (z-axis rotation)
		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.z = std::atan2(siny_cosp, cosy_cosp);

		return angles;
	}
}

DirectX::SimpleMath::Matrix Transform::GetLocalMatrix() const
{
	return Matrix::CreateScale(scale)
		* Matrix::CreateFromQuaternion(rotation)
		* Matrix::CreateTranslation(position);
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
	ImGui::DragFloat3("Position##Transform", &t.position.x, 0.1f);

	{
		auto euler = ToEulerAngles(DirectX::SimpleMath::Quaternion(t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w)) * (180.f / DirectX::XM_PI);

		float rot[] = { euler.x, euler.y, euler.z };

		// the "##Transform" ensures that you can use the name "x" in multiple lables
		ImGui::DragFloat3("Rotation##Transform", &rot[0], 0.1f);

		auto quat = ToQuaternion(DirectX::SimpleMath::Vector3(rot[0], rot[1], rot[2]) * (DirectX::XM_PI / 180.f));
		t.rotation.x = quat.x;
		t.rotation.y = quat.y;
		t.rotation.z = quat.z;
		t.rotation.w = quat.w;
	}

	// the "##Transform" ensures that you can use the name "x" in multiple lables
	ImGui::DragFloat3("Scale##Transform", &t.scale.x, 0.1f);
}