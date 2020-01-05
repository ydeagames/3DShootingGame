#include "pch.h"
#include "WidgetDND.h"
#include <Framework/Components/Transform.h>
#include <Framework/Context/SceneManager.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/MyProject.h>

namespace Widgets
{
	void WidgetDND::SetDragDropPayload()
	{
		ImGui::SetDragDropPayload("DND_Hierarchy", this, sizeof(WidgetDND));
	}

	const WidgetDND* WidgetDND::AcceptDragDropPayload()
	{
		if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_Hierarchy"))
			return static_cast<const WidgetDND*>(payload->Data);
		return nullptr;
	}

	void WidgetDND::DragDropTarget(entt::registry& reg, entt::entity parent)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const WidgetDND * data = AcceptDragDropPayload())
			{
				if (data->regptr == &reg)
				{
					if (data->entity != parent)
						reg.get<Transform>(data->entity).parent = parent;
				}
				else
				{
					if (auto scenefrom = GameContext::Get<SceneManager>().GetSceneOrNullRegistry(data->regptr))
					{
						auto& reg0 = scenefrom->registry;

						std::vector<entt::entity> src;
						std::vector<entt::entity> dst;
						auto rec0 = [&](auto& e, auto& rec) mutable -> void {
							src.push_back(e);
							reg0.view<Transform>().each([&](auto entity, Transform& component) {
								if (component.parent == e)
									rec(entity, rec);
								});
						};
						rec0(data->entity, rec0);

						ECS::AllComponents::CloneComponents(reg0, src, reg, dst);
						ECS::AllComponents::UpdateReferences(reg, src, dst);

						if (!dst.empty())
							reg.get<Transform>(dst.front()).parent = parent;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}
