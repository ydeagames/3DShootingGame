// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-16 22:30:22 +0900

#include "pch.h"
#include "Widgets.h"
#include "WidgetDND.h"
#include <Framework/ECS/Scene.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Transform.h>
#include <Utilities/WindowsUtils.h>
#include <Framework/Context/SceneManager.h>
#include <Framework/Context/ApplicationHandler.h>
#include <Framework/ECS/Project.h>

namespace Widgets
{
	void Hierarchy(Scene& scene, entt::entity& e, entt::entity& e0)
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

		std::string title = "Scene [" + scene.info.name + "]";
		ImGuiTreeNodeFlags node_flags = ((e == entt::null) ? ImGuiTreeNodeFlags_Selected : 0)
			| (nodes.empty() ? ImGuiTreeNodeFlags_Leaf : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen;
		bool opened = ImGui::TreeNodeEx(title.c_str(), node_flags, title.c_str());

		if (ImGui::IsItemClicked())
			e = entt::null;

		WidgetDND::DragDropTarget(reg, entt::null);

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
								WidgetDND(reg, node.id).SetDragDropPayload();
								ImGui::Text(node.name.c_str());
								ImGui::EndDragDropSource();
							}

							WidgetDND::DragDropTarget(reg, node.id);

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

	void Inspector(Scene& scene, entt::entity& e)
	{
		MM::renderImGui(scene.registry, e);

		ImGui::Separator();

		auto& editorTag = GameContext::Get<MM::ImGuiEntityTagEditor<entt::registry>>();
		editorTag.renderImGui(scene.registry, e);

		ImGui::Separator();

		auto& editorComponent = GameContext::Get<MM::ImGuiEntityComponentEditor<entt::registry>>();
		editorComponent.renderImGui(scene.registry, e);
	}

	void SceneControl(Scene& scene)
	{
		ImGui::LabelText("Scene", scene.info.name.c_str());

		if (ImGui::Button("Save Scene"))
		{
			scene.Save();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			scene.Load();
		}
		if (ImGui::Button("Scene Additive"))
		{
			GameContext::Get<SceneManager>().LoadScene(SceneInfo::CreateFromName("Untitled"), LoadSceneMode::Additive);
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove Scene"))
		{
			if (GameContext::Get<SceneManager>().GetSceneCount() > 1)
				Scene::Destroy(scene);
		}
		if (ImGui::Button("Save Scene As"))
		{
			std::string location;
			if (WindowsUtils::SaveDialog("scene.json", "Scene Files", location))
			{
				scene.info = { WindowsUtils::GetFileName(location, "scene.json"), location };
				scene.Save();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene"))
		{
			std::string location;
			if (WindowsUtils::OpenDialog("scene.json", "Scene Files", location))
			{
				scene.info = { WindowsUtils::GetFileName(location, "scene.json"), location };
				scene.Load();
			}
		}
	}

	void EntityControl(Scene& scene, entt::entity& e)
	{
		auto& reg = scene.registry;

		{
			int iid = (e == entt::null) ? -1 : int(reg.entity(e));
			if (ImGui::InputInt("ID", &iid))
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

				ECS::AllComponents::CloneComponents(reg, src, reg, dst);
				ECS::AllComponents::UpdateReferences(reg, src, dst);

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

				ECS::AllComponents::SaveEntity(location, reg, src);
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

				ECS::AllComponents::LoadEntity(location, reg, src, dst);
				ECS::AllComponents::UpdateReferences(reg, src, dst);

				auto& e0 = *dst.begin();
				if (!ImGui::GetIO().KeyShift)
					e = e0;
			}
		}
	}

	namespace AllWidgets
	{
		void Render()
		{
			auto& editorState = GameContext::Get<EntityEditorState>();

			if (editorState.editorEnabled)
			{
				auto scene = GameContext::Get<SceneManager>().GetSceneOrNull(editorState.scene);

				if (scene)
				{
					if (ImGui::Begin("Entity Editor", &editorState.editorEnabled))
					{
						ImGui::Text("Scene (%s)", scene->info.name.c_str());

						Widgets::Inspector(*scene, editorState.current);
					}
					ImGui::End();
				}

				if (ImGui::Begin("Hierarchy", &editorState.editorEnabled))
				{
					bool playing = GameContext::Get<ApplicationHandler>().IsPlaying();
					if (ImGui::Checkbox("Is Playing", &playing))
						GameContext::Get<ApplicationHandler>().SetPlaying(playing);

					GameContext::Get<SceneManager>().ForEachScenes([&](Scene& hScene)
						{
							ImGui::PushID(ImGuiID(&hScene));

							entt::entity e = (editorState.scene == &hScene) ? editorState.current : entt::null;
							entt::entity e0 = (editorState.scene == &hScene) ? editorState.prev : entt::null;

							std::stringstream sb;
							sb << "Scene" << " (" << hScene.info.name << ")";
							if (ImGui::CollapsingHeader(sb.str().c_str()))
							{
								ImGui::BeginGroup();

								Widgets::SceneControl(hScene);

								ImGui::Separator();

								Widgets::EntityControl(hScene, e);

								ImGui::Separator();

								Widgets::Hierarchy(hScene, e, e0);

								ImGui::EndGroup();

								if (ImGui::IsItemActive())
								{
									editorState.scene = &hScene;
									editorState.current = e;
									editorState.prev = e0;
								}
							}

							ImGui::PopID();
						});
				}
				ImGui::End();
			}
		}
	}
}


