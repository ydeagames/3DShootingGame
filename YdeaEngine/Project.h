#pragma once

namespace ECS
{
	class AllComponents
	{
	public:
		static HMODULE library;
		static void Load();
		static void Unload();
		
	public:
		static void InitializeEvents();
		static void InitializeEditorComponents(MM::ImGuiEntityEditor<entt::registry, entt::registry::component_type>& editor);
		static void InitializeEditorTags(MM::ImGuiEntityEditor<entt::registry, entt::registry::tag_type>& editor);
		static void InitializeDependency(entt::registry& reg);
		static void InitializeLifecycleEvents(entt::registry& reg);
		static void UpdateReferences(entt::registry& reg, const std::vector<entt::entity>& srcs, const std::vector<entt::entity>& dsts);
		static void CloneComponents(entt::registry& srcreg, const std::vector<entt::entity>& srcs, entt::registry& dstreg, std::vector<entt::entity>& dsts);
		static bool LoadScene(const std::string& location, entt::registry& scene, const std::function<void(entt::registry&)>& initFunc);
		static bool SaveScene(const std::string& location, const entt::registry& scene);
		static bool LoadEntity(const std::string& location, entt::registry& scene, std::vector<entt::entity>& srcs, std::vector<entt::entity>& dsts);
		static bool SaveEntity(const std::string& location, const entt::registry& scene, const std::vector<entt::entity>& srcs);
	};
}
