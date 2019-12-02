#pragma once
#include "Serialize.h"

namespace ECS
{
	template<typename Registry, typename Base>
	class Dependency
	{
	private:
		Registry& registry;

	public:
		Dependency(Registry& registry)
			: registry(registry)
		{
		}

	private:
		template<typename T>
		class Creation
		{
		public:
			void on(Registry& registry, typename Registry::entity_type entity)
			{
				registry.get_or_assign<T>(entity);
			}
		};

		template<typename T>
		class Deletion
		{
		public:
			void on(Registry& registry, typename Registry::entity_type entity)
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

	template<typename Registry>
	class ReferenceResolver
	{
	public:
		using map_type = std::unordered_map<typename Registry::entity_type, typename Registry::entity_type>;

	private:
		Registry& registry;
		const map_type& map;

	public:
		ReferenceResolver(Registry& registry, const map_type& map)
			: registry(registry)
			, map(map)
		{
		}

	public:
		typename Registry::entity_type& operator()(typename Registry::entity_type& ref)
		{
			if (map.find(ref) != map.end())
				ref = map.at(ref);
			return ref;
		}
	};

	template<typename Registry>
	class ComponentDependency
	{
	private:
		template<typename Component, typename = decltype(&Component::Dependency<Component>)>
		static void DependsOn0(int, Registry & reg)
		{
			Dependency<Registry, Component> dep(reg);
			Component::Dependency(dep);
		}

		template<typename Component>
		static void DependsOn0(bool, Registry& reg)
		{
		}

	public:
		template<typename Component>
		static void DependsOn(Registry& reg)
		{
			DependsOn0<Component>(0, reg);
		}
	};

	template<typename Registry>
	class ComponentReference
	{
	private:
		template<typename Component, typename = decltype(&Component::Reference<Component>)>
		static void Resolve0(int, Registry & reg, const std::vector<typename Registry::entity_type> & srcs, const std::vector<typename Registry::entity_type> & dsts)
		{
			ReferenceResolver<Registry>::map_type map;
			for (auto srcitr = srcs.begin(), dstitr = dsts.begin(); srcitr != srcs.end() && dstitr != dsts.end(); ++srcitr, ++dstitr)
				map.insert(std::make_pair(*srcitr, *dstitr));
			ReferenceResolver<Registry> ref(reg, map);
			for (auto& dst : dsts)
				if (reg.has<Component>(dst))
					reg.get<Component>(dst).Reference(ref);
		}

		template<typename Component>
		static void Resolve0(bool, Registry& reg, const std::vector<typename Registry::entity_type>& srcs, const std::vector<typename Registry::entity_type>& dsts)
		{
		}

	public:
		template<typename Component>
		static void Resolve(Registry& reg, const std::vector<typename Registry::entity_type>& srcs, const std::vector<typename Registry::entity_type>& dsts)
		{
			Resolve0<Component>(0, reg, srcs, dsts);
		}
	};

	template<typename Registry>
	class ComponentGui
	{
	private:
		template<typename Component, typename = decltype(&Component::EditorGui)>
		static void EditorWidget0(int, Registry & reg, MM::ImGuiEntityEditor<Registry> & editor)
		{
			editor.registerComponentWidgetFn(
				reg.type<Component>(),
				[](auto& registry, auto entity) {
					registry.get<Component>(entity).EditorGui();
				});
		}

		template<typename Component>
		static void EditorWidget0(bool, Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
		}

	public:
		template<typename Component>
		static void EditorWidget(Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
			EditorWidget0<Component>(0, reg, editor);
		}
	};

	// Declaration of a template
	template<typename Registry, typename Components, typename Tags, typename Events>
	class ComponentManager;

	template<typename Registry, typename... Components, typename... Tags, typename... Events>
	class ComponentManager<Registry, std::tuple<Components...>, std::tuple<Tags...>, std::tuple<Events...>>
	{
	private:
		template<typename Event>
		static void InitializeEvent()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (Event::Register<Components>(), 0)... };
			(void)accumulator;
		}

		template<typename Registry, typename Component>
		static void InitializeEditorComponent(Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
			editor.registerTrivial<Component>(reg, ECS::IdentifierResolver::name<Component>());
			ComponentGui<Registry>::EditorWidget<Component>(reg, editor);
		}

		template<typename Registry, typename Component>
		static void InitializeDependency(Registry& reg)
		{
			ComponentDependency<Registry>::DependsOn<Component>(reg);
		}

		template<typename Registry, typename Component>
		static void UpdateReference(Registry& reg, const std::vector<typename Registry::entity_type>& srcs, const std::vector<typename Registry::entity_type>& dsts)
		{
			ComponentReference<Registry>::Resolve<Component>(reg, srcs, dsts);
		}

	public:
		static void InitializeEvents()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeEvent<Events>(), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void InitializeEditorComponents(Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeEditorComponent<Registry, Components>(reg, editor), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void InitializeDependency(Registry& reg)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeDependency<Registry, Components>(reg), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void InitializeLifecycleEvents(Registry& reg)
		{
			ECS::LifecycleEvents<Registry>::Lifecycle<Components...>(reg);
		}

		template<typename Registry>
		static void UpdateReferences(Registry& reg, const std::vector<typename Registry::entity_type>& srcs, const std::vector<typename Registry::entity_type>& dsts)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (UpdateReference<Registry, Components>(reg, srcs, dsts), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void CloneComponents(Registry& reg, const std::vector<typename Registry::entity_type>& srcs, std::vector<typename Registry::entity_type>& dsts)
		{
			ComponentClone<Registry>::Clone<Components...>(reg, srcs, dsts);
		}

		template<typename Registry, typename RegistryInitializer>
		static bool LoadScene(const std::string& location, Registry& scene, RegistryInitializer initFunc)
		{
			std::ifstream storage(location);
			auto snap = scene.restore();
			initFunc(scene);
			return ObjectSerializer<Registry, std::tuple<Components...>, std::tuple<Tags...>>::Import(storage, snap);
		}

		template<typename Registry>
		static bool SaveScene(const std::string& location, const Registry& scene)
		{
			std::ofstream storage(location);
			return ObjectSerializer<Registry, std::tuple<Components...>, std::tuple<Tags...>>::Export(storage, scene.snapshot());
		}

		template<typename Registry>
		static bool LoadEntity(const std::string& location, Registry& scene, std::vector<typename Registry::entity_type>& srcs, std::vector<typename Registry::entity_type>& dsts)
		{
			std::ifstream storage(location);
			if (storage)
			{
				try
				{
					cereal::JSONInputArchive archive(storage);
					EntityImporter<Registry> serializer(archive);
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

		template<typename Registry>
		static bool SaveEntity(const std::string& location, const Registry& scene, const std::vector<typename Registry::entity_type>& srcs)
		{
			std::ofstream storage(location);
			if (storage)
			{
				try
				{
					cereal::JSONOutputArchive archive(storage);
					EntityExporter<Registry> serializer(archive);
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