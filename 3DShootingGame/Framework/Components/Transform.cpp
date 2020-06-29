// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-03 17:52:30 +0900
// Modified: 2020-01-05 22:47:30 +0900

#include "pch.h"
#include "Transform.h"
#include <Framework/ECS/GameContext.h>
#include <Utilities/Math3DUtils.h>
#include <Framework/ImGui/WidgetDND.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

Transform::Transform()
	: localPosition(Vector3::Zero)
	, localRotation(Quaternion::Identity)
	, localScale(Vector3::One)
{
}

void Transform::Awake()
{
	transformResolver = &GameContext::Get<TransformResolverContext>().GetResolver(*gameObject.registry);
}

DirectX::SimpleMath::Vector3 Transform::GetPosition() const
{
	auto p = gameObject.GetParentEntity();
	if (p != entt::null && transformResolver)
		return Vector3::Transform(localPosition, transformResolver->Resolve(gameObject.Wrap(p)));
	else
		return localPosition;
}

void Transform::SetPosition(const DirectX::SimpleMath::Vector3& value)
{
	auto p = gameObject.GetParentEntity();
	if (p != entt::null && transformResolver)
		localPosition = Vector3::Transform(value, transformResolver->Resolve(gameObject.Wrap(p)).Invert());
	else
		localPosition = value;
}

DirectX::SimpleMath::Quaternion Transform::GetRotation() const
{
	auto p = gameObject.GetParentEntity();
	if (p != entt::null && transformResolver)
		return localRotation * Quaternion::CreateFromRotationMatrix(transformResolver->Resolve(gameObject.Wrap(p)));
	else
		return localRotation;
}

void Transform::SetRotation(const DirectX::SimpleMath::Quaternion& value)
{
	auto p = gameObject.GetParentEntity();
	if (p != entt::null && transformResolver)
		localRotation = value * Quaternion::CreateFromRotationMatrix(transformResolver->Resolve(gameObject.Wrap(p)).Invert());
	else
		localRotation = value;
}

DirectX::SimpleMath::Vector3 Transform::GetLossyScale() const
{
	auto p = gameObject.GetParentEntity();
	if (p != entt::null && transformResolver)
	{
		Vector3 parentPosition;
		Quaternion parentRotation;
		Vector3 parentScale;
		transformResolver->Resolve(gameObject.Wrap(p)).Decompose(parentScale, parentRotation, parentPosition);
		auto mat = Matrix::CreateScale(parentScale) * Matrix::CreateFromQuaternion(parentRotation);
		return Vector3::Transform(localScale, mat);
	}
	else
		return localScale;
}

void Transform::SetLossyScale(const DirectX::SimpleMath::Vector3& value)
{
	auto p = gameObject.GetParentEntity();
	if (p != entt::null && transformResolver)
	{
		Vector3 parentPosition;
		Quaternion parentRotation;
		Vector3 parentScale;
		transformResolver->Resolve(gameObject.Wrap(p)).Decompose(parentScale, parentRotation, parentPosition);
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
	if (transformResolver)
		return transformResolver->Resolve(gameObject);
	return GetLocalMatrix();
}

void Transform::EditorGui()
{
	auto& t = *this;
	auto& reg = *gameObject.registry;

	std::string tmpname = t.name;
	tmpname.resize(16);
	if (ImGui::InputText("Name", &tmpname[0], tmpname.size()))
		t.name = std::string(tmpname.c_str());

	{
		auto& e = t.parent;
		int iid = (e == entt::null) ? -1 : int(reg.entity(e));
		if (ImGui::InputInt("Parent", &iid))
			if (iid < 0)
				e = entt::null;
			else
			{
				auto id = entt::entity(iid);
				e = id < reg.size() ? (id | reg.current(id) << entt::entt_traits<entt::entity>::entity_shift) : id;
			}

		if (ImGui::BeginDragDropTarget())
		{
			if (const auto * data = Widgets::WidgetDND::AcceptDragDropPayload())
			{
				if (data->regptr == &reg)
					e = data->entity;
			}
			ImGui::EndDragDropTarget();
		}
	}

	ImGui::Checkbox("Static", &isStatic);

	// the "" ensures that you can use the name "x" in multiple lables
	ImGui::DragFloat3("Position", &t.localPosition.x, 0.1f);

	{
		auto euler = Math3DUtils::ToEulerAngles(DirectX::SimpleMath::Quaternion(t.localRotation.x, t.localRotation.y, t.localRotation.z, t.localRotation.w)) * (180.f / DirectX::XM_PI);

		float rot[] = { euler.x, euler.y, euler.z };

		// the "" ensures that you can use the name "x" in multiple lables
		if (ImGui::DragFloat3("Rotation", &rot[0], 0.1f))
		{
			auto quat = Math3DUtils::ToQuaternion(DirectX::SimpleMath::Vector3(rot[0], rot[1], rot[2]) * (DirectX::XM_PI / 180.f));
			t.localRotation.x = quat.x;
			t.localRotation.y = quat.y;
			t.localRotation.z = quat.z;
			t.localRotation.w = quat.w;
		}
	}

	// the "" ensures that you can use the name "x" in multiple lables
	ImGui::DragFloat3("Scale", &t.localScale.x, 0.1f);
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

void TransformResolverContext::ClearCache()
{
	for (auto& resolver : resolvers)
		resolver.second.ClearCache();
}

TransformResolver& TransformResolverContext::GetResolver(entt::registry& registry)
{
	return resolvers[&registry];
}


