// for the license, see the end of the file
#pragma once

#include <functional>

//#include <entt/entt.hpp>
//#include <imgui.h>

// if you have font awesome or something comparable you can set this to a wastebin
#ifndef ESS_IMGUI_ENTT_E_E_DELETE_COMP_STR
#define ESS_IMGUI_ENTT_E_E_DELETE_COMP_STR "-"
#endif

namespace MM {

	template<typename Registry>
	void renderImGui(Registry& ecs, typename Registry::entity_type& e) {
		ImGui::TextUnformatted("editing:");
		ImGui::SameLine();

		//ImGuiWidgets::Entity(e, ecs, true);
		if (ecs.valid(e)) {
			ImGui::Text("id: %d, v: %d", ecs.entity(e), ecs.version(e));
		}
		else {
			ImGui::Text("INVALID ENTITY");
		}
	}

	template<typename Registry, typename Type>
	class ImGuiEntityEditor {
	public:
		using component_type = Type;
		using component_event = std::function<void(Registry&, typename Registry::entity_type)>;
		using component_predicate = std::function<bool(Registry&, typename Registry::entity_type)>;

	public:
		entt::SparseSet<component_type> _component_types;
		entt::SparseSet<component_type, std::string> _component_names;
		entt::SparseSet<component_type, component_event> _component_widget;
		entt::SparseSet<component_type, component_event> _component_create;
		entt::SparseSet<component_type, component_event> _component_destroy;
		entt::SparseSet<component_type, component_predicate> _component_has;
		entt::SparseSet<component_type, component_predicate> _component_candidate;

	public:
		// call this (or registerTrivial) before any of the other register functions
		void registerComponentType(component_type ct) {
			if (!_component_types.has(ct)) {
				_component_types.construct(ct);
			}
		}

		// register a name to be displayed for the component
		void registerComponentName(component_type ct, const std::string& name) {
			_component_names.construct(ct, name);
		}

		// register a callback to a function displaying a component. using imgui
		void registerComponentWidgetFn(component_type ct, component_event&& fn) {
			_component_widget.construct(ct, std::forward<component_event>(fn));
		}

		// register a callback to create a component, if none, you wont be able to create it in the editor
		void registerComponentCreateFn(component_type ct, component_event&& fn) {
			_component_create.construct(ct, std::forward<component_event>(fn));
		}

		// register a callback to delete a component, if none, you wont be able to delete it in the editor
		void registerComponentDestroyFn(component_type ct, component_event&& fn) {
			_component_destroy.construct(ct, std::forward<component_event>(fn));
		}

		// register a predicate to show
		void registerComponentHasFn(component_type ct, component_predicate&& fn) {
			_component_has.construct(ct, std::forward<component_predicate>(fn));
		}

		// register a predicate to show
		void registerComponentCandidateFn(component_type ct, component_predicate&& fn) {
			_component_candidate.construct(ct, std::forward<component_predicate>(fn));
		}

		// registers the component_type, name, create and destroy for rather trivial types
		template<typename T>
		void registerComponentTrivial(const std::string& name) {
			registerComponentType(Registry::type<T>());
			registerComponentName(Registry::type<T>(), name);
			registerComponentCreateFn(Registry::type<T>(),
				[](Registry& ecs, typename Registry::entity_type e) {
					ecs.template assign<T>(e);
				});
			registerComponentDestroyFn(Registry::type<T>(),
				[](Registry& ecs, typename Registry::entity_type e) {
					ecs.template remove<T>(e);
				});
			registerComponentHasFn(Registry::type<T>(),
				[](Registry& ecs, typename Registry::entity_type e) {
					return ecs.template has<T>(e);
				});
			registerComponentCandidateFn(Registry::type<T>(),
				[](Registry& ecs, typename Registry::entity_type e) {
					return !ecs.template has<T>(e);
				});
		}

		// registers the component_type, name, create and destroy for rather trivial types
		template<typename T>
		void registerTagTrivial(const std::string& name) {
			registerComponentType(Registry::type<T>(entt::tag_t{}));
			registerComponentName(Registry::type<T>(entt::tag_t{}), name);
			registerComponentCreateFn(Registry::type<T>(entt::tag_t{}),
				[](Registry& ecs, typename Registry::entity_type e) {
					ecs.template assign<T>(entt::tag_t{}, e);
				});
			registerComponentDestroyFn(Registry::type<T>(entt::tag_t{}),
				[](Registry& ecs, typename Registry::entity_type e) {
					ecs.template remove<T>();
				});
			registerComponentHasFn(Registry::type<T>(entt::tag_t{}),
				[](Registry& ecs, typename Registry::entity_type e) {
					return ecs.template has<T>(entt::tag_t{}, e);
				});
			registerComponentCandidateFn(Registry::type<T>(entt::tag_t{}),
				[](Registry& ecs, typename Registry::entity_type e) {
					return !ecs.template has<T>();
				});
		}
	};

	template<typename Registry>
	class ImGuiEntityComponentEditor {
	public:
		using editor_type = typename ImGuiEntityEditor<Registry, typename Registry::component_type>;
		editor_type editor;

	private:
		using component_type = typename editor_type::component_type;

	public:
		// calls all the ImGui functions
		// call this every frame
		void renderImGui(Registry& ecs, typename Registry::entity_type& e) {
			ImGui::Text("Components");
			if (e != entt::null && ecs.valid(e)) {
				ImGui::PushID(this);

				std::vector<component_type> has_not;
				for (auto ct : editor._component_types) {
					if (editor._component_has.has(ct) && editor._component_has.get(ct)(ecs, e)) {
						ImGui::PushID(ImGuiID(ct));

						// delete component button
						if (editor._component_destroy.has(ct)) {
							std::string button_label = ESS_IMGUI_ENTT_E_E_DELETE_COMP_STR "##";
							button_label += std::to_string(ct);

							if (ImGui::Button(button_label.c_str())) {
								editor._component_destroy.get(ct)(ecs, e);

								ImGui::PopID();
								continue; // early out to prevent access to deleted data
							}
							else {
								ImGui::SameLine();
							}
						}

						std::string label;
						if (editor._component_names.has(ct)) {
							label = editor._component_names.get(ct);
						}
						else {
							label = "unnamed component (";
							label += std::to_string(ct);
							label += ")";
						}

						if (ImGui::CollapsingHeader(label.c_str())) {
							ImGui::Indent(30.f);

							if (editor._component_widget.has(ct)) {
								editor._component_widget.get(ct)(ecs, e);
							}
							else {
								ImGui::TextDisabled("missing widget to display component!");
							}

							ImGui::Unindent(30.f);
						}

						ImGui::PopID();
					}
					if (editor._component_candidate.has(ct) && editor._component_candidate.get(ct)(ecs, e)) {
						has_not.push_back(ct);
					}
				}

				if (!has_not.empty()) {
					if (ImGui::Button("+ Add Component")) {
						ImGui::OpenPopup("add component");
					}

					if (ImGui::BeginPopup("add component")) {
						ImGui::TextUnformatted("available:");
						ImGui::Separator();

						for (auto ct : has_not) {
							if (editor._component_create.has(ct)) {
								std::string label;
								if (editor._component_names.has(ct)) {
									label = editor._component_names.get(ct);
								}
								else {
									label = "unnamed component (";
									label += std::to_string(ct);
									label += ")";
								}

								label += "##"; label += std::to_string(ct); // better but optional

								if (ImGui::Selectable(label.c_str())) {
									editor._component_create.get(ct)(ecs, e);
								}
							}
						}

						ImGui::EndPopup();
					}
				}

				ImGui::PopID();
			}
		}
	};

	template<typename Registry>
	class ImGuiEntityTagEditor {
	public:
		using editor_type = typename ImGuiEntityEditor<Registry, typename Registry::tag_type>;
		editor_type editor;

	private:
		using component_type = typename editor_type::component_type;

	public:
		// calls all the ImGui functions
		// call this every frame
		void renderImGui(Registry& ecs, typename Registry::entity_type& e) {
			ImGui::Text("Tags");
			if (e != entt::null && ecs.valid(e)) {
				std::vector<component_type> has_not;
				for (auto ct : editor._component_types) {
					if (editor._component_has.has(ct) && editor._component_has.get(ct)(ecs, e)) {
						ImGui::PushID(ImGuiID(ct));

						// delete component button
						if (editor._component_destroy.has(ct)) {
							std::string button_label = ESS_IMGUI_ENTT_E_E_DELETE_COMP_STR "##";
							button_label += std::to_string(ct);

							if (ImGui::Button(button_label.c_str())) {
								editor._component_destroy.get(ct)(ecs, e);

								ImGui::PopID();
								continue; // early out to prevent access to deleted data
							}
							else {
								ImGui::SameLine();
							}
						}

						std::string label;
						if (editor._component_names.has(ct)) {
							label = editor._component_names.get(ct);
						}
						else {
							label = "unnamed tag (";
							label += std::to_string(ct);
							label += ")";
						}

						if (ImGui::CollapsingHeader(label.c_str())) {
							ImGui::Indent(30.f);

							if (editor._component_widget.has(ct)) {
								editor._component_widget.get(ct)(ecs, e);
							}
							else {
								ImGui::TextDisabled("missing widget to display tag!");
							}

							ImGui::Unindent(30.f);
						}

						ImGui::PopID();
					}
					if (editor._component_candidate.has(ct) && editor._component_candidate.get(ct)(ecs, e)) {
						has_not.push_back(ct);
					}
				}

				if (!has_not.empty()) {
					if (ImGui::Button("+ Add Tag")) {
						ImGui::OpenPopup("add tag");
					}

					if (ImGui::BeginPopup("add tag")) {
						ImGui::TextUnformatted("available:");
						ImGui::Separator();

						for (auto ct : has_not) {
							if (editor._component_create.has(ct)) {
								std::string label;
								if (editor._component_names.has(ct)) {
									label = editor._component_names.get(ct);
								}
								else {
									label = "unnamed tag (";
									label += std::to_string(ct);
									label += ")";
								}

								label += "##"; label += std::to_string(ct); // better but optional

								if (ImGui::Selectable(label.c_str())) {
									editor._component_create.get(ct)(ecs, e);
								}
							}
						}

						ImGui::EndPopup();
					}
				}
			}
		}
	};
} // MM

// MIT License

// Copyright (c) 2019 Erik Scholz
// Edited by 2019 YdeaGames

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
