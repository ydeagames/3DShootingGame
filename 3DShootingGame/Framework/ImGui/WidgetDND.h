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

