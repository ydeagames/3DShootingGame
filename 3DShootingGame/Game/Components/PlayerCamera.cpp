#include "pch.h"
#include "PlayerCamera.h"
#include <Framework/Components/Transform.h>
#include <Framework/ImGui/WidgetDND.h>
#include <Utilities/Math3DUtils.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void AbstractFollower::EditorGui()
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

void TrackingFollower::Update()
{
	if (gameObject.registry->valid(target))
	{
		auto obj = gameObject.Wrap(target);
		if (obj.HasComponent<Transform>())
		{
			auto& transformDst = obj.GetComponent<Transform>();
			auto& transformSrc = gameObject.GetComponent<Transform>();

			auto pos = transformSrc.position;
			auto posTarget = transformDst.position;
			auto delta = posTarget - pos;

			// yŽ²‰ñ“]‚Ì‚Ý
			delta.y = 0;

			auto pos2 = posTarget - (Math3DUtils::Normalized(delta) * radius);

			pos2.y = posTarget.y + height;

			transformSrc.position = pos2;
			transformSrc.rotation = Math3DUtils::LookAt(pos2, posTarget);
		}
	}
}

void TrackingFollower::EditorGui()
{
	AbstractFollower::EditorGui();
	ImGui::DragFloat("Radius", &radius);
	ImGui::DragFloat("Height", &height);
}
