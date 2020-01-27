#include "pch.h"
#include "Goal.h"
#include <Framework/Components/Transform.h>
#include <Utilities/Math3DUtils.h>
#include <Framework/ImGui/WidgetDND.h>
#include <Framework/Physics/Rigidbody.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/SceneManager.h>
#include "Framework/SceneManagement/SceneTransitions.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Goal::Start()
{
}

void Goal::Update()
{
	if (gameObject.registry->valid(target))
	{
		auto obj = gameObject.Wrap(target);
		if (obj.HasComponent<Transform>())
		{
			auto& transform = gameObject.GetComponent<Transform>();
			auto& transformTarget = obj.GetComponent<Transform>();
			BoundingBox bounds(transform.position, transform.localScale * .5f);
			BoundingBox boundsTarget(transformTarget.position, transformTarget.localScale);

			if (bounds.Intersects(boundsTarget))
				if (!m_goaled)
				{
					m_goaled = true;
					GameContext::Get<SceneManager>().LoadSceneTransition(SceneInfo::CreateFromName("ResultScene"), SceneTransitions::CreateRuleTransition(3));
				}
		}
	}
}

void Goal::OnDestroy()
{
}

void Goal::EditorGui()
{
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
