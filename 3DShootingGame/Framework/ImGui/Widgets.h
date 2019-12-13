#pragma once

class GameContext;
class Scene;

namespace Widgets
{
	class EntityEditorState
	{
	public:
		void* scene = nullptr;
		entt::entity prev = entt::null;
		entt::entity current = entt::null;
		bool editorEnabled = false;
	};

	void Hierarchy(Scene& scene, entt::entity& e, entt::entity& e0);
	void Inspector(Scene& scene, entt::entity& e);
	void SceneControl(Scene& scene);
	void EntityControl(Scene& scene, entt::entity& e);

	namespace AllWidgets
	{
		void Render();
	}
};
