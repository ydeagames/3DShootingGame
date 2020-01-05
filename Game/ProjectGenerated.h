#pragma once

namespace ECS
{
	namespace AllComponents
	{
		extern "C" __declspec(dllexport) void InitializeEvents();
		extern "C" __declspec(dllexport) void InitializeEditorComponents(MM::ImGuiEntityEditor<entt::registry, entt::registry::component_type>& editor);
		extern "C" __declspec(dllexport) void InitializeEditorTags(MM::ImGuiEntityEditor<entt::registry, entt::registry::tag_type>& editor);
		extern "C" __declspec(dllexport) void InitializeDependency(entt::registry& reg);
		extern "C" __declspec(dllexport) void InitializeLifecycleEvents(entt::registry& reg);
		extern "C" __declspec(dllexport) void UpdateReferences(entt::registry& reg, const std::vector<entt::entity>& srcs, const std::vector<entt::entity>& dsts);
		extern "C" __declspec(dllexport) void CloneComponents(entt::registry& srcreg, const std::vector<entt::entity>& srcs, entt::registry& dstreg, std::vector<entt::entity>& dsts);
		extern "C" __declspec(dllexport) bool LoadScene(const std::string& location, entt::registry& scene, const std::function<void(entt::registry&)>& initFunc);
		extern "C" __declspec(dllexport) bool SaveScene(const std::string& location, const entt::registry& scene);
		extern "C" __declspec(dllexport) bool LoadEntity(const std::string& location, entt::registry& scene, std::vector<entt::entity>& srcs, std::vector<entt::entity>& dsts);
		extern "C" __declspec(dllexport) bool SaveEntity(const std::string& location, const entt::registry& scene, const std::vector<entt::entity>& srcs);
	}
}
