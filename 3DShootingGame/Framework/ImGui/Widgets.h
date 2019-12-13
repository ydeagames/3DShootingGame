#pragma once

class GameContext;
class Scene;

namespace Widgets
{
	class EntityEditorState
	{
	public:
		entt::entity prev = entt::null;
		entt::entity current = entt::null;
		bool hierarchyEnabled = false;
		bool inspectorEnabled = false;
	};

	void Hierarchy(Scene& scene);
	void Inspector(Scene& scene);
	void SceneControl(Scene& scene);
	void EntityControl(Scene& scene);

	namespace AllWidgets
	{
		void Render(Scene& scene);
	}
};
