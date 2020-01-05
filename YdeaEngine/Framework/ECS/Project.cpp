#include "pch.h"
#include "Project.h"

namespace ECS
{
	HMODULE AllComponents::library = nullptr;

	void AllComponents::Load()
	{
		if (!library)
			library = LoadLibraryW(L"Game.dll");
	}

	void AllComponents::Unload()
	{
		if (library)
			FreeLibrary(library);
	}

	void AllComponents::InitializeEvents()
	{
		const auto func = reinterpret_cast<decltype(&InitializeEvents)>(GetProcAddress(library, "InitializeEvents"));
		func();
	}

	void AllComponents::InitializeEditorComponents(
		MM::ImGuiEntityEditor<entt::registry, entt::registry::component_type>& editor)
	{
		const auto func = reinterpret_cast<decltype(&InitializeEditorComponents)>(GetProcAddress(library, "InitializeEditorComponents"));
		func(editor);
	}

	void AllComponents::InitializeEditorTags(MM::ImGuiEntityEditor<entt::registry, entt::registry::tag_type>& editor)
	{
		const auto func = reinterpret_cast<decltype(&InitializeEditorTags)>(GetProcAddress(library, "InitializeEditorTags"));
		func(editor);
	}

	void AllComponents::InitializeDependency(entt::registry& reg)
	{
		const auto func = reinterpret_cast<decltype(&InitializeDependency)>(GetProcAddress(library, "InitializeDependency"));
		func(reg);
	}

	void AllComponents::InitializeLifecycleEvents(entt::registry& reg)
	{
		const auto func = reinterpret_cast<decltype(&InitializeLifecycleEvents)>(GetProcAddress(library, "InitializeLifecycleEvents"));
		func(reg);
	}

	void AllComponents::UpdateReferences(entt::registry& reg, const std::vector<entt::entity>& srcs,
		const std::vector<entt::entity>& dsts)
	{
		const auto func = reinterpret_cast<decltype(&UpdateReferences)>(GetProcAddress(library, "UpdateReferences"));
		func(reg, srcs, dsts);
	}

	void AllComponents::CloneComponents(entt::registry& srcreg, const std::vector<entt::entity>& srcs,
		entt::registry& dstreg, std::vector<entt::entity>& dsts)
	{
		const auto func = reinterpret_cast<decltype(&CloneComponents)>(GetProcAddress(library, "CloneComponents"));
		func(srcreg, srcs, dstreg, dsts);
	}

	bool AllComponents::LoadScene(const std::string& location, entt::registry& scene,
		const std::function<void(entt::registry&)>& initFunc)
	{
		const auto func = reinterpret_cast<decltype(&LoadScene)>(GetProcAddress(library, "LoadScene"));
		return func(location, scene, initFunc);
	}

	bool AllComponents::SaveScene(const std::string& location, const entt::registry& scene)
	{
		const auto func = reinterpret_cast<decltype(&SaveScene)>(GetProcAddress(library, "SaveScene"));
		return func(location, scene);
	}

	bool AllComponents::LoadEntity(const std::string& location, entt::registry& scene, std::vector<entt::entity>& srcs,
		std::vector<entt::entity>& dsts)
	{
		const auto func = reinterpret_cast<decltype(&LoadEntity)>(GetProcAddress(library, "LoadEntity"));
		return func(location, scene, srcs, dsts);
	}

	bool AllComponents::SaveEntity(const std::string& location, const entt::registry& scene,
		const std::vector<entt::entity>& srcs)
	{
		const auto func = reinterpret_cast<decltype(&SaveEntity)>(GetProcAddress(library, "SaveEntity"));
		return func(location, scene, srcs);
	}
}
