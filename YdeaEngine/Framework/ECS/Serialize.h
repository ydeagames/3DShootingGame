#pragma once

class GameContext;

namespace ECS
{
	class IdentifierResolver
	{
	private:
		template<typename T, typename = decltype(&T::Identifier)>
		static const char* name0(int)
		{
			return T::Identifier;
		}

		template<typename T>
		static const char* name0(bool)
		{
			const char* name = typeid(T).name();
			auto ptr = std::strchr(name, ' ');
			if (ptr == nullptr)
				return name;
			return ptr + 1;
		}

	public:
		template<typename T>
		static const char* name()
		{
			return name0<T>(0);
		}
	};

	template <typename Entity>
	class ObjectSnapshotLoader
	{
	private:
		cereal::JSONInputArchive& archive;
		Entity stack = 0;

	public:
		ObjectSnapshotLoader(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity& entity)
		{
			if (stack == 0)
			{
				cereal::size_type size;
				archive.loadSize(size);
				entity = stack = static_cast<Entity>(size);
			}
			else
			{
				stack--;
				archive(entity);
			}
		}
	};

	template <typename Entity>
	class ObjectTagSnapshotLoader
	{
	private:
		cereal::JSONInputArchive& archive;
		Entity stack = 0;

	public:
		ObjectTagSnapshotLoader(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity& entity)
		{
			if (stack == 0)
			{
				entity = stack = 1;
			}
			else
			{
				assert(false);
			}
		}

		template<typename T>
		void operator()(Entity& entity, T& tag)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("tag", tag));
				archive.finishNode();
			}
		}

	private:
		template<typename Tag>
		void tag0(const entt::SnapshotLoader<Entity>& loader) {
			const char* name = IdentifierResolver::name<Tag>();
			if (archive.hasName(name))
			{
				archive.setNextName(name);
				loader.tag<Tag>(*this);
			}
		}

	public:
		template<typename... Tags>
		void tag(const entt::SnapshotLoader<Entity>& loader) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (tag0<Tags>(loader), 0)... };
			(void)accumulator;
		}
	};

	template <typename Entity>
	class ObjectComponentSnapshotLoader
	{
	private:
		cereal::JSONInputArchive& archive;
		Entity stack = 0;

	public:
		ObjectComponentSnapshotLoader(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity& entity)
		{
			if (stack == 0)
			{
				archive.startNode();
				cereal::size_type size;
				archive.loadSize(size);
				entity = stack = static_cast<Entity>(size);
			}
			else
			{
				assert(false);
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

		template<typename T>
		void operator()(Entity& entity, T& component)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("component", component));
				archive.finishNode();
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

	private:
		template<typename Component>
		void component0(const entt::SnapshotLoader<Entity>& loader) {
			const char* name = IdentifierResolver::name<Component>();
			if (archive.hasName(name))
			{
				archive.setNextName(name);
				loader.component<Component>(*this);
			}
		}

	public:
		template<typename... Components>
		void component(const entt::SnapshotLoader<Entity>& loader) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Components>(loader), 0)... };
			(void)accumulator;
		}
	};

	template <typename Entity>
	class ObjectSnapshot
	{
	private:
		cereal::JSONOutputArchive& archive;
		Entity stack = 0;

	public:
		ObjectSnapshot(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity entity)
		{
			if (stack == 0)
			{
				stack = entity;
			}
			else
			{
				stack--;
				archive(entity);
			}
		}
	};

	template <typename Entity>
	class ObjectTagSnapshot
	{
	private:
		cereal::JSONOutputArchive& archive;
		Entity stack = 0;

	public:
		ObjectTagSnapshot(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity entity)
		{
			if (stack == 0)
			{
				stack = entity;
			}
			else
			{
				assert(false);
			}
		}

		template<typename T>
		void operator()(Entity entity, const T& tag)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("tag", tag));
				archive.finishNode();
			}
		}

	private:
		template<typename Tag>
		void tag0(const entt::Snapshot<Entity>& saver) {
			archive.setNextName(IdentifierResolver::name<Tag>());
			saver.tag<Tag>(*this);
		}

	public:
		template<typename... Tags>
		void tag(const entt::Snapshot<Entity>& saver) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (tag0<Tags>(saver), 0)... };
			(void)accumulator;
		}
	};

	template <typename Entity>
	class ObjectComponentSnapshot
	{
	private:
		cereal::JSONOutputArchive& archive;
		Entity stack = 0;

	public:
		ObjectComponentSnapshot(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity entity)
		{
			if (stack == 0)
			{
				archive.startNode();
				archive.makeArray();
				stack = entity;
			}
			else
			{
				assert(false);
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

		template<typename T>
		void operator()(Entity entity, const T& component)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("component", component));
				archive.finishNode();
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

	private:
		template<typename Component>
		void component0(const entt::Snapshot<Entity>& saver) {
			archive.setNextName(IdentifierResolver::name<Component>());
			saver.component<Component>(*this);
		}

		template<typename Component, typename It>
		void component0(const entt::Snapshot<Entity>& saver, It first, It last) {
			archive.setNextName(IdentifierResolver::name<Component>());
			saver.component<Component>(*this, first, last);
		}

	public:
		template<typename... Components>
		void component(const entt::Snapshot<Entity>& saver) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Components>(saver), 0)... };
			(void)accumulator;
		}

		template<typename... Components, typename It>
		void component(const entt::Snapshot<Entity>& saver, It first, It last) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Components>(saver, first, last), 0)... };
			(void)accumulator;
		}
	};

	template<typename Components, typename Tags>
	class ObjectSerializer;

	template<typename... Components, typename... Tags>
	class ObjectSerializer<std::tuple<Components...>, std::tuple<Tags...>>
	{
	public:
		template<typename Snap>
		static bool Import(std::istream& storage, Snap&& snap)
		{
			if (storage)
			{
				try
				{
					cereal::JSONInputArchive archive{ storage };
					ObjectSnapshotLoader<entt::entity> oarchive(archive);
					ObjectTagSnapshotLoader<entt::entity> tarchive(archive);
					ObjectComponentSnapshotLoader<entt::entity> carchive(archive);
					{
						{
							archive.setNextName("entities");
							archive.startNode();
							{
								archive.setNextName("created");
								archive.startNode();
								snap.entities(oarchive);
								archive.finishNode();
							}
							{
								archive.setNextName("destroyed");
								archive.startNode();
								snap.destroyed(oarchive);
								archive.finishNode();
							}
							archive.finishNode();
						}
						{
							archive.setNextName("tags");
							archive.startNode();
							tarchive.tag<Tags...>(snap);
							archive.finishNode();
						}
						{
							archive.setNextName("components");
							archive.startNode();
							carchive.component<Components...>(snap);
							archive.finishNode();
						}
						//snap.orphans();
					}
					return true;
				}
				catch (cereal::Exception e)
				{
					// —áŠO
					std::cerr << e.what() << std::endl;
				}
			}
			return false;
		}

		template<typename Snap>
		static bool Export(std::ostream& storage, Snap&& snap)
		{
			if (storage)
			{
				try
				{
					// output finishes flushing its contents when it goes out of scope
					cereal::JSONOutputArchive archive{ storage };
					ObjectSnapshot<entt::entity> oarchive(archive);
					ObjectTagSnapshot<entt::entity> tarchive(archive);
					ObjectComponentSnapshot<entt::entity> carchive(archive);
					{
						{
							archive.setNextName("entities");
							archive.startNode();
							{
								archive.setNextName("created");
								archive.startNode();
								archive.makeArray();
								snap.entities(oarchive);
								archive.finishNode();
							}
							{
								archive.setNextName("destroyed");
								archive.startNode();
								archive.makeArray();
								snap.destroyed(oarchive);
								archive.finishNode();
							}
							archive.finishNode();
						}
						{
							archive.setNextName("tags");
							archive.startNode();
							tarchive.tag<Tags...>(snap);
							archive.finishNode();
						}
						{
							archive.setNextName("components");
							archive.startNode();
							carchive.component<Components...>(snap);
							archive.finishNode();
						}
					}
					return true;
				}
				catch (cereal::Exception e)
				{
					// —áŠO
					std::cerr << e.what() << std::endl;
				}
			}
			return false;
		}

		template<typename Snap>
		static bool Export(std::ostream& storage, Snap&& snap, const std::vector<entt::entity>& entities)
		{
			if (storage)
			{
				try
				{
					// output finishes flushing its contents when it goes out of scope
					cereal::JSONOutputArchive archive{ storage };
					ObjectComponentSnapshot<entt::entity> carchive(archive);
					{
						auto first = entities.begin();
						auto last = entities.end();
						{
							archive.setNextName("entities");
							archive.startNode();
							{
								archive.setNextName("created");
								archive.startNode();
								archive.makeArray();
								std::for_each(first, last, archive);
								archive.finishNode();
							}
							{
								archive.setNextName("destroyed");
								archive.startNode();
								archive.makeArray();
								archive.finishNode();
							}
							archive.finishNode();
						}
						{
							archive.setNextName("tags");
							archive.startNode();
							archive.finishNode();
						}
						{
							archive.setNextName("components");
							archive.startNode();
							carchive.component<Components...>(snap, first, last);
							archive.finishNode();
						}
					}
					return true;
				}
				catch (cereal::Exception e)
				{
					// —áŠO
					std::cerr << e.what() << std::endl;
				}
			}
			return false;
		}
	};

	class EntityExporter
	{
	private:
		cereal::JSONOutputArchive& archive;

	public:
		EntityExporter(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	private:
		template<typename Component>
		void component0(const entt::registry& reg, entt::entity entity)
		{
			archive.setNextName(IdentifierResolver::name<Component>());
			if (reg.has<Component>(entity))
			{
				auto& component = reg.get<Component>(entity);
				archive(component);
			}
		}

	public:
		template<typename... Components>
		void component(const entt::registry& reg, entt::entity entity)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Components>(reg, entity), 0)... };
			(void)accumulator;
		}

		template<typename... Components>
		void components(const entt::registry& reg, const std::vector<entt::entity>& srcs)
		{
			{
				archive.setNextName("entities");
				archive.startNode();
				{
					archive(cereal::make_nvp("created", srcs));
				}
				archive.finishNode();
			}
			{
				archive.setNextName("objects");
				archive.startNode();
				archive.makeArray();
				for (auto& entity : srcs)
				{
					archive.startNode();
					component<Components...>(reg, entity);
					archive.finishNode();
				}
				archive.finishNode();
			}
		}
	};

	class EntityImporter
	{
	private:
		cereal::JSONInputArchive& archive;

	public:
		EntityImporter(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	private:
		template<typename Component>
		void component0(entt::registry& reg, entt::entity entity)
		{
			const char* name = IdentifierResolver::name<Component>();
			if (archive.hasName(name))
			{
				archive.setNextName(name);
				auto& component = reg.accommodate<Component>(entity);
				archive(component);
			}
		}

	public:
		template<typename... Components>
		void component(entt::registry& reg, entt::entity& entity)
		{
			entity = reg.create();
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Components>(reg, entity), 0)... };
			(void)accumulator;
		}

		template<typename... Components>
		void components(entt::registry& reg, std::vector<entt::entity>& srcs, std::vector<entt::entity>& dsts)
		{
			{
				archive.setNextName("entities");
				archive.startNode();
				{
					archive(cereal::make_nvp("created", srcs));
				}
				archive.finishNode();
			}
			{
				archive.setNextName("objects");
				archive.startNode();
				//archive.makeArray();
				cereal::size_type size;
				archive.loadSize(size);
				for (cereal::size_type i = 0; i < size; i++)
				{
					archive.startNode();
					entt::entity entity;
					component<Components...>(reg, entity);
					dsts.push_back(entity);
					archive.finishNode();
				}
				archive.finishNode();
			}
		}
	};

	class ComponentClone
	{
	public:
		template<typename... Components>
		static void Clone(entt::registry& srcreg, entt::entity src, entt::registry& dstreg, entt::entity& dst)
		{
			std::stringstream buffer;
			{
				cereal::JSONOutputArchive archive(buffer);
				EntityExporter serializer(archive);
				serializer.component<Components...>(srcreg, src);
			}
			{
				cereal::JSONInputArchive archive(buffer);
				EntityImporter serializer(archive);
				serializer.component<Components...>(dstreg, dst);
			}
		}

		template<typename... Components>
		static void Clone(entt::registry& srcreg, const std::vector<entt::entity>& srcs, entt::registry& dstreg, std::vector<entt::entity>& dsts)
		{
			std::vector<entt::entity> srcsResult;
			std::stringstream buffer;
			{
				cereal::JSONOutputArchive archive(buffer);
				EntityExporter serializer(archive);
				serializer.components<Components...>(srcreg, srcs);
			}
			std::cout << buffer.str() << std::endl;
			{
				cereal::JSONInputArchive archive(buffer);
				EntityImporter serializer(archive);
				serializer.components<Components...>(dstreg, srcsResult, dsts);
			}
		}
	};
}