#include "pch.h"
#include "ProjectGenerated.h"
#include <Framework/StandardBuildSettings.h>
#include <Game/Components/AllComponents.h>
#include <Game/Events/AllEvents.h>
#include <Game/Tags/AllTags.h>

namespace
{
	template<typename ... T_args>
	struct connect;

	template<typename ... T1, typename ... T2>
	struct connect<std::tuple<T1...>, std::tuple<T2...>>
	{
		typedef std::tuple<T1..., T2...> type;
	};

	using Delegate = ECS::ComponentManager<
		connect<
			StandardBuildSettings::Components,
			BuildSettings::Components
		>::type,
		connect<
			StandardBuildSettings::Tags,
			BuildSettings::Tags
		>::type,
		connect<
			StandardBuildSettings::Events,
			BuildSettings::Events
		>::type
	>;
}

namespace ECS
{
	namespace AllComponents
	{
		void InitializeEvents()
		{
			Delegate::InitializeEvents();
		}

		void InitializeEditorComponents(
			MM::ImGuiEntityEditor<entt::registry, entt::registry::component_type>& editor)
		{
			Delegate::InitializeEditorComponents(editor);
		}

		void InitializeEditorTags(MM::ImGuiEntityEditor<entt::registry, entt::registry::tag_type>& editor)
		{
			Delegate::InitializeEditorTags(editor);
		}

		void InitializeDependency(entt::registry& reg)
		{
			Delegate::InitializeDependency(reg);
		}

		void InitializeLifecycleEvents(entt::registry& reg)
		{
			Delegate::InitializeLifecycleEvents(reg);
		}

		void UpdateReferences(entt::registry& reg, const std::vector<entt::entity>& srcs,
			const std::vector<entt::entity>& dsts)
		{
			Delegate::UpdateReferences(reg, srcs, dsts);
		}

		void CloneComponents(entt::registry& srcreg, const std::vector<entt::entity>& srcs,
			entt::registry& dstreg, std::vector<entt::entity>& dsts)
		{
			Delegate::CloneComponents(srcreg, srcs, dstreg, dsts);
		}

		bool LoadScene(const std::string& location, entt::registry& scene,
			const std::function<void(entt::registry&)>& initFunc)
		{
			return Delegate::LoadScene(location, scene, initFunc);
		}

		bool SaveScene(const std::string& location, const entt::registry& scene)
		{
			return Delegate::SaveScene(location, scene);
		}

		bool LoadEntity(const std::string& location, entt::registry& scene, std::vector<entt::entity>& srcs,
			std::vector<entt::entity>& dsts)
		{
			return Delegate::LoadEntity(location, scene, srcs, dsts);
		}

		bool SaveEntity(const std::string& location, const entt::registry& scene,
			const std::vector<entt::entity>& srcs)
		{
			return Delegate::SaveEntity(location, scene, srcs);
		}
	}
}
