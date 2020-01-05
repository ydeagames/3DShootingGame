#pragma once
#include "Serialize.h"

namespace ECS
{
	template<typename Base>
	class Dependency
	{
	private:
		entt::registry& registry;

	public:
		Dependency(entt::registry& registry)
			: registry(registry)
		{
		}

	private:
		template<typename T>
		class Creation
		{
		public:
			void on(entt::registry& registry, entt::entity entity)
			{
				registry.get_or_assign<T>(entity);
			}
		};

		template<typename T>
		class Deletion
		{
		public:
			void on(entt::registry& registry, entt::entity entity)
			{
				registry.reset<T>(entity);
			}
		};

		template<typename Dependency>
		void DependsOns()
		{
			Creation<Dependency> cre;
			registry.construction<Base>().connect<Creation<Dependency>, & Creation<Dependency>::on>(&cre);
			Deletion<Base> del;
			registry.destruction<Dependency>().connect<Deletion<Base>, & Deletion<Base>::on>(&del);
		}

	public:
		template<typename... Dependency>
		void DependsOn()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (DependsOns<Dependency>(), 0)... };
			(void)accumulator;
		}
	};

	class ReferenceResolver
	{
	public:
		using map_type = std::unordered_map<entt::entity, entt::entity>;

	private:
		entt::registry& registry;
		const map_type& map;

	public:
		ReferenceResolver(entt::registry& registry, const map_type& map)
			: registry(registry)
			, map(map)
		{
		}

	public:
		entt::entity& operator()(entt::entity& ref)
		{
			if (map.find(ref) != map.end())
				ref = map.at(ref);
			return ref;
		}
	};

	class ComponentDependency
	{
	private:
		template<typename Component, typename = decltype(&Component::template Dependency<Component>)>
		static void DependsOn0(int, entt::registry & reg)
		{
			Dependency<Component> dep(reg);
			Component::Dependency(dep);
		}

		template<typename Component>
		static void DependsOn0(bool, entt::registry& reg)
		{
		}

	public:
		template<typename Component>
		static void DependsOn(entt::registry& reg)
		{
			DependsOn0<Component>(0, reg);
		}
	};

	class ComponentReference
	{
	private:
		template<typename Component, typename = decltype(&Component::template Reference<Component>)>
		static void Resolve0(int, entt::registry & reg, const std::vector<entt::entity> & srcs, const std::vector<entt::entity> & dsts)
		{
			ReferenceResolver::map_type map;
			for (auto srcitr = srcs.begin(), dstitr = dsts.begin(); srcitr != srcs.end() && dstitr != dsts.end(); ++srcitr, ++dstitr)
				map.insert(std::make_pair(*srcitr, *dstitr));
			ReferenceResolver ref(reg, map);
			for (auto& dst : dsts)
				if (reg.has<Component>(dst))
					reg.get<Component>(dst).Reference(ref);
		}

		template<typename Component>
		static void Resolve0(bool, entt::registry& reg, const std::vector<entt::entity>& srcs, const std::vector<entt::entity>& dsts)
		{
		}

	public:
		template<typename Component>
		static void Resolve(entt::registry& reg, const std::vector<entt::entity>& srcs, const std::vector<entt::entity>& dsts)
		{
			Resolve0<Component>(0, reg, srcs, dsts);
		}
	};

	class ComponentGui
	{
	private:
		template<typename Component, typename Editor, typename = decltype(&Component::EditorGui)>
		static void EditorComponentWidget0(int, Editor & editor)
		{
			editor.registerComponentWidgetFn(
				entt::registry::type<Component>(),
				[](auto& registry, auto entity) {
					registry.get<Component>(entity).EditorGui();
				});
		}

		template<typename Component, typename Editor>
		static void EditorComponentWidget0(bool, Editor& editor)
		{
		}

	private:
		template<typename Tag, typename Editor, typename = decltype(&Tag::EditorGui)>
		static void EditorTagWidget0(int, Editor & editor)
		{
			editor.registerComponentWidgetFn(
				entt::registry::type<Tag>(entt::tag_t{}),
				[](auto& registry, auto entity) {
					registry.get<Tag>().EditorGui();
				});
		}

		template<typename Tag, typename Editor>
		static void EditorTagWidget0(bool, Editor& editor)
		{
		}

	public:
		template<typename Component, typename Editor>
		static void EditorComponentWidget(Editor& editor)
		{
			EditorComponentWidget0<Component>(0, editor);
		}

		template<typename Tag, typename Editor>
		static void EditorTagWidget(Editor& editor)
		{
			EditorTagWidget0<Tag>(0, editor);
		}
	};

	// Declaration of a template
	template<typename Components, typename Tags, typename Events>
	class ComponentManager;

	template<typename... Components, typename... Tags, typename... Events>
	class ComponentManager<std::tuple<Components...>, std::tuple<Tags...>, std::tuple<Events...>>
	{
	private:
		template<typename Event>
		static void InitializeEvent()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (Event::Register<Components>(), 0)... };
			(void)accumulator;
		}

		template<typename Component>
		static void InitializeEditorComponent(MM::ImGuiEntityEditor<entt::registry, entt::registry::component_type>& editor)
		{
			editor.registerComponentTrivial<Component>(ECS::IdentifierResolver::name<Component>());
			ComponentGui::EditorComponentWidget<Component>(editor);
		}

		template<typename Tag>
		static void InitializeEditorTag(MM::ImGuiEntityEditor<entt::registry, entt::registry::tag_type>& editor)
		{
			editor.registerTagTrivial<Tag>(ECS::IdentifierResolver::name<Tag>());
			ComponentGui::EditorTagWidget<Tag>(editor);
		}

		template<typename Component>
		static void InitializeDependency(entt::registry& reg)
		{
			ComponentDependency::DependsOn<Component>(reg);
		}

		template<typename Component>
		static void UpdateReference(entt::registry& reg, const std::vector<entt::entity>& srcs, const std::vector<entt::entity>& dsts)
		{
			ComponentReference::Resolve<Component>(reg, srcs, dsts);
		}

	public:
		static void InitializeEvents()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeEvent<Events>(), 0)... };
			(void)accumulator;
		}

		static void InitializeEditorComponents(MM::ImGuiEntityEditor<entt::registry, entt::registry::component_type>& editor)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeEditorComponent<Components>(editor), 0)... };
			(void)accumulator;
		}

		static void InitializeEditorTags(MM::ImGuiEntityEditor<entt::registry, entt::registry::tag_type>& editor)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeEditorTag<Tags>(editor), 0)... };
			(void)accumulator;
		}

		static void InitializeDependency(entt::registry& reg)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeDependency<Components>(reg), 0)... };
			(void)accumulator;
		}

		static void InitializeLifecycleEvents(entt::registry& reg)
		{
			ECS::LifecycleEvents::Lifecycle<Components...>(reg);
		}

		static void UpdateReferences(entt::registry& reg, const std::vector<entt::entity>& srcs, const std::vector<entt::entity>& dsts)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (UpdateReference<Components>(reg, srcs, dsts), 0)... };
			(void)accumulator;
		}

		static void CloneComponents(entt::registry& srcreg, const std::vector<entt::entity>& srcs, entt::registry& dstreg, std::vector<entt::entity>& dsts)
		{
			ComponentClone::Clone<Components...>(srcreg, srcs, dstreg, dsts);
		}

		static bool LoadScene(const std::string& location, entt::registry& scene, std::function<void(entt::registry&)> initFunc)
		{
			std::ifstream storage(location);
			auto snap = scene.restore();
			initFunc(scene);
			return ObjectSerializer<std::tuple<Components...>, std::tuple<Tags...>>::Import(storage, snap);
		}

		static bool SaveScene(const std::string& location, const entt::registry& scene)
		{
			std::ofstream storage(location);
			return ObjectSerializer<std::tuple<Components...>, std::tuple<Tags...>>::Export(storage, scene.snapshot());
		}

		static bool LoadEntity(const std::string& location, entt::registry& scene, std::vector<entt::entity>& srcs, std::vector<entt::entity>& dsts)
		{
			std::ifstream storage(location);
			if (storage)
			{
				try
				{
					cereal::JSONInputArchive archive(storage);
					EntityImporter serializer(archive);
					serializer.components<Components...>(scene, srcs, dsts);
					return true;
				}
				catch (cereal::Exception e)
				{
					// —áŠO
				}
			}
			return false;
		}

		static bool SaveEntity(const std::string& location, const entt::registry& scene, const std::vector<entt::entity>& srcs)
		{
			std::ofstream storage(location);
			if (storage)
			{
				try
				{
					cereal::JSONOutputArchive archive(storage);
					EntityExporter serializer(archive);
					serializer.components<Components...>(scene, srcs);
					return true;
				}
				catch (cereal::Exception)
				{
					// —áŠO
				}
			}
			return false;
		}
	};
}