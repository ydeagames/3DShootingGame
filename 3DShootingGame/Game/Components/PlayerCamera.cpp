#include "pch.h"
#include "PlayerCamera.h"
#include <Framework/Components/Transform.h>
#include <Framework/ImGui/WidgetDND.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void TranslateFollower::Update()
{
	if (gameObject.registry->valid(target))
	{
		auto obj = gameObject.Wrap(target);
		if (obj.HasComponent<Transform>())
		{
			auto& transformDst = obj.GetComponent<Transform>();

			auto& transformSrc = gameObject.GetComponent<Transform>();
			transformSrc.position = Vector3::Lerp(transformSrc.position, transformDst.position, movePercent);
		}
	}
}

void TranslateFollower::EditorGui()
{
	ImGui::DragFloat("Move Lerp Percent", &movePercent);

	{
		auto& reg = *gameObject.registry;
		auto& e = target;
		int iid = (e == entt::null) ? -1 : int(reg.entity(e));
		if (ImGui::InputInt("Target", &iid))
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
}

void RotateFollower::Update()
{
	if (gameObject.registry->valid(target))
	{
		auto obj = gameObject.Wrap(target);
		if (obj.HasComponent<Transform>())
		{
			auto& transformDst = obj.GetComponent<Transform>();

			auto& transformSrc = gameObject.GetComponent<Transform>();
			transformSrc.rotation = Quaternion::Lerp(transformSrc.rotation, transformDst.rotation, movePercent);
		}
	}
}

void RotateFollower::EditorGui()
{
	ImGui::DragFloat("Move Lerp Percent", &movePercent);

	{
		auto& reg = *gameObject.registry;
		auto& e = target;
		int iid = (e == entt::null) ? -1 : int(reg.entity(e));
		if (ImGui::InputInt("Target", &iid))
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
}
