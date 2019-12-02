#include "pch.h"
#include "Widgets.h"
#include <Framework/ECS/Scene.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Components.h>
#include <Framework/Components/AllComponents.h>
#include <Framework/WindowsUtils.h>

namespace Widgets
{
	void Hierarchy(Scene& scene)
	{
		class Node
		{
		public:
			entt::entity id;
			std::string name;
			entt::entity parent = entt::null;
			bool hasloop = false;
			bool hasparent = false;
			Transform* transform = nullptr;
			std::vector<entt::entity> children;

		public:
			Node(const entt::entity& id)
				: id(id)
			{}
		};

		auto& reg = scene.registry;
		auto& editorState = GameContext::Get<EntityEditorState>();
		auto& e0 = editorState.prev;
		auto& e = editorState.current;

		//std::unordered_set<entt::entity> checknodes;
		entt::SparseSet<entt::entity, Node> nodes;
		reg.each([&](auto entity) {
			Node node{ entity };

			std::stringstream sb;
			sb << "[" << reg.entity(entity) << "]";
			if (reg.has<Transform>(entity))
			{
				auto& transform = reg.get<Transform>(entity);
				node.transform = &transform;
				node.parent = transform.parent;
				sb << " " << transform.name;
			}
			node.name = sb.str();
			nodes.construct(entity, std::move(node));
			});

		for (auto& node : nodes)
		{
			auto loop = [&nodes](entt::entity id) {
				auto slow = id, fast = id;
				while (true)
				{
					//if (!nodes.has(slow))
					//	return false;
					slow = nodes.get(slow).parent;
					//if (!nodes.has(fast))
					//	return false;
					fast = nodes.get(fast).parent;
					if (!nodes.has(fast))
						return false;
					fast = nodes.get(fast).parent;
					if (!nodes.has(slow) || !nodes.has(fast))
						return false;
					if (slow == fast)
						return true;
				}
			};
			node.hasparent = reg.valid(node.parent) && nodes.has(node.parent);
			if (node.id == node.parent || loop(node.id))
				node.hasloop = true;
			else if (reg.valid(node.parent) && nodes.has(node.parent))
				nodes.get(node.parent).children.push_back(node.id);
		}

		std::string title = "Scene [" + scene.name + "]";
		ImGuiTreeNodeFlags node_flags = ((e == entt::null) ? ImGuiTreeNodeFlags_Selected : 0)
			| (nodes.empty() ? ImGuiTreeNodeFlags_Leaf : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen;
		bool opened = ImGui::TreeNodeEx(title.c_str(), node_flags, title.c_str());

		if (ImGui::IsItemClicked())
			e = entt::null;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_Hierarchy"))
			{
				auto data = *(static_cast<const entt::entity*>(payload->Data));
				reg.get<Transform>(data).parent = entt::null;
			}
			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			for (auto& node : nodes)
			{
				if (node.hasloop || !node.hasparent)
				{
					auto rec0 = [&](Node& node, auto& rec) mutable -> void {
						ImGui::Indent(-5.f);
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
							ImGuiTreeNodeFlags node_flags = ((e == node.id) ? ImGuiTreeNodeFlags_Selected : 0)
								| (node.children.empty() ? ImGuiTreeNodeFlags_Leaf : 0)
								| ImGuiTreeNodeFlags_OpenOnArrow
								| ImGuiTreeNodeFlags_OpenOnDoubleClick
								| ImGuiTreeNodeFlags_DefaultOpen;
							bool opened = ImGui::TreeNodeEx(node.name.c_str(), node_flags, node.name.c_str());
							ImGui::PopStyleVar();

							ImGui::PushID(node.name.c_str());
							if (ImGui::BeginPopupContextItem())
							{
								ImGui::EndPopup();
							}
							ImGui::PopID();

							if (ImGui::IsItemClicked())
							{
								e0 = e;
								e = node.id;
							}

							if (ImGui::BeginDragDropSource())
							{
								e = e0;
								ImGui::SetDragDropPayload("DND_Hierarchy", &node.id, sizeof(entt::entity));
								ImGui::Text(node.name.c_str());
								ImGui::EndDragDropSource();
							}

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_Hierarchy"))
								{
									auto data = *(static_cast<const entt::entity*>(payload->Data));
									if (data != node.id && data != node.parent)
										reg.get<Transform>(data).parent = node.id;
								}
								ImGui::EndDragDropTarget();
							}

							if (node.parent != entt::null && (node.hasloop || !node.hasparent))
							{
								if (node.transform)
								{
									ImGui::SameLine();
									if (ImGui::SmallButton("Fix Transform"))
										node.transform->parent = entt::null;
								}
							}

							if (opened)
							{
								if (!node.hasloop)
									for (auto& nodeindex : node.children)
									{
										rec(nodes.get(nodeindex), rec);
									}
								ImGui::TreePop();
							}
						}
						ImGui::Unindent(-5.f);
					};
					rec0(node, rec0);
				}
			}
			ImGui::TreePop();
		}
	}

	void Inspector(Scene& scene)
	{
		auto& reg = scene.registry;
		auto& editor = GameContext::Get<MM::ImGuiEntityEditor<entt::registry>>();
		auto& editorState = GameContext::Get<EntityEditorState>();
		editor.renderImGui(reg, editorState.current);
	}

	void SceneControl(Scene& scene)
	{
		ImGui::LabelText("Scene", scene.name.c_str());

		if (ImGui::Button("Save Scene"))
		{
			scene.Save();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			scene.Load();
		}
		if (ImGui::Button("Save Scene As"))
		{
			std::string location;
			if (WindowsUtils::SaveDialog("scene.json", "Scene Files", location))
			{
				scene.location = location;
				scene.name = WindowsUtils::GetFileName(location, "scene.json");
				scene.Save();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene"))
		{
			std::string location;
			if (WindowsUtils::OpenDialog("scene.json", "Scene Files", location))
			{
				scene.location = location;
				scene.name = WindowsUtils::GetFileName(location, "scene.json");
				scene.Load();
			}
		}
	}

	void EntityControl(Scene& scene)
	{
		auto& reg = scene.registry;
		auto& editorState = GameContext::Get<EntityEditorState>();
		auto& e = editorState.current;
		{
			int iid = (e == entt::null) ? -1 : int(reg.entity(e));
			ImGui::InputInt("ID", &iid);
			if (iid < 0)
				e = entt::null;
			else
			{
				auto id = entt::entity(iid);
				e = id < reg.size() ? (id | reg.current(id) << entt::entt_traits<entt::entity>::entity_shift) : id;
			}
		}

		if (ImGui::Button("New"))
		{
			auto e0 = reg.create();
			Transform t;
			if (reg.valid(e))
			{
				auto parent = reg.has<Transform>(e) ? reg.get<Transform>(e).parent : entt::null;
				if (reg.valid(parent))
					t.parent = parent;
			}
			reg.assign<Transform>(e0, std::move(t));
			if (!ImGui::GetIO().KeyShift)
				e = e0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			auto rec0 = [&](auto& e, auto& rec) mutable -> void {
				reg.view<Transform>().each([&](auto entity, Transform& component) {
					if (component.parent == e)
						rec(entity, rec);
					});
				reg.destroy(e);
			};
			rec0(e, rec0);
		}
		if (ImGui::Button("New Child"))
		{
			auto e0 = reg.create();
			Transform t;
			if (reg.valid(e))
				t.parent = e;
			reg.assign<Transform>(e0, std::move(t));
			if (!ImGui::GetIO().KeyShift)
				e = e0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Duplicate"))
		{
			if (reg.valid(e))
			{
				std::vector<entt::entity> src;
				std::vector<entt::entity> dst;
				auto rec0 = [&](auto& e, auto& rec) mutable -> void {
					src.push_back(e);
					reg.view<Transform>().each([&](auto entity, Transform& component) {
						if (component.parent == e)
							rec(entity, rec);
						});
				};
				rec0(e, rec0);

				Components::CloneComponents(reg, src, dst);
				Components::UpdateReferences(reg, src, dst);

				auto& e0 = *dst.begin();
				if (!ImGui::GetIO().KeyShift)
					e = e0;
			}
		}
		if (ImGui::Button("Export"))
		{
			std::string location;
			if (WindowsUtils::SaveDialog("prefab.json", "Prefab Files", location))
			{
				std::vector<entt::entity> src;
				auto rec0 = [&](auto& e, auto& rec) mutable -> void {
					src.push_back(e);
					reg.view<Transform>().each([&](auto entity, Transform& component) {
						if (component.parent == e)
							rec(entity, rec);
						});
				};
				rec0(e, rec0);

				Components::SaveEntity(location, reg, src);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Import"))
		{
			std::string location;
			if (WindowsUtils::OpenDialog("prefab.json", "Prefab Files", location))
			{
				std::vector<entt::entity> src;
				std::vector<entt::entity> dst;

				Components::LoadEntity(location, reg, src, dst);
				Components::UpdateReferences(reg, src, dst);

				auto& e0 = *dst.begin();
				if (!ImGui::GetIO().KeyShift)
					e = e0;
			}
		}
	}

	namespace AllWidgets
	{
		void Initialize(Scene& scene)
		{
			auto& reg = scene.registry;
			GameContext::Register<EntityEditorState>();
			auto& editor = GameContext::Register<MM::ImGuiEntityEditor<entt::registry>>();
			Components::InitializeEditorComponents(reg, editor);
		}

		void Render(Scene& scene)
		{
			Widgets::Inspector(scene);

			if (ImGui::Begin("Hierarchy"))
			{
				Widgets::SceneControl(scene);

				ImGui::Separator();

				Widgets::EntityControl(scene);

				ImGui::Separator();

				Widgets::Hierarchy(scene);

				ImGui::End();
			}
		}
	}
}
