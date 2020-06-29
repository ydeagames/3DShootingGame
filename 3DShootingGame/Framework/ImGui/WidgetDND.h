// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-16 22:30:22 +0900
// Modified: 2019-12-16 22:30:22 +0900

#pragma once

namespace Widgets
{
	class WidgetDND
	{
	public:
		void* regptr;
		entt::entity entity;

	public:
		WidgetDND(entt::registry& reg, entt::entity entity)
			: regptr(&reg)
			, entity(entity)
		{
		}

		WidgetDND()
			: regptr(nullptr)
			, entity(entt::null)
		{
		}

	public:
		void SetDragDropPayload();
		static const WidgetDND* AcceptDragDropPayload();
		static void DragDropTarget(entt::registry& reg, entt::entity parent);
	};
}



