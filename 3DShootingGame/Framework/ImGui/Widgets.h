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
	};

	void Hierarchy(Scene& scene);
	void Inspector(Scene& scene);
	void SceneControl(Scene& scene);
	void EntityControl(Scene& scene);

	namespace AllWidgets
	{
		void Initialize(Scene& scene);
		void Render(Scene& scene);
	}
};
