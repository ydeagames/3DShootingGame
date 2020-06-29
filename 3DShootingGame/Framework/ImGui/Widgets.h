// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-13 22:30:57 +0900

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


