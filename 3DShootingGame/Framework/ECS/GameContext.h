#pragma once
#include <Common/DeviceResources.h>

class GameCamera;

class GameContext final
{
private:
	class GameContextImpl
	{
	private:
		class IHolder
		{
		public:
			virtual ~IHolder() = default;
		};

		template<typename T>
		class Holder : public IHolder
		{
		public:
			std::unique_ptr<T> data;
		};

		using ctx_family = entt::Family<struct InternalContextFamily>;
		std::vector<std::unique_ptr<IHolder>> pools;

		template<typename Component>
		inline Holder<Component>& holder()
		{
			const auto ctype = ctx_family::type<Component>();
			return static_cast<Holder<Component>&>(*pools[ctype]);
		}

		template<typename Component>
		inline const Holder<Component>& holder() const
		{
			const auto ctype = ctx_family::type<Component>();
			return static_cast<const Holder<Component>&>(*pools.at(ctype));
		}

	public:
		template<typename Component, typename... Args>
		Component& Register(Args&& ... args)
		{
			assert(!Has<Component>() && "Already Registered");
			const auto ctype = ctx_family::type<Component>();
			if (!(ctype < pools.size()))
				pools.resize(ctype + 1);
			if (!pools[ctype])
				pools[ctype] = std::make_unique<Holder<Component>>();
			return *(holder<Component>().data = std::make_unique<Component>(std::forward<Args>(args)...));
		}

		template<typename Component>
		bool Has() const
		{
			const auto ctype = ctx_family::type<Component>();
			return ctype < pools.size() && pools[ctype] && holder<Component>().data;
		}

		template<typename Component>
		void Remove()
		{
			holder<Component>().data = nullptr;
		}

		template<typename Component>
		inline Component& Get()
		{
			return *holder<Component>().data;
		}
	};

	class Impl
	{
	public:
		GameContextImpl context;

	public:
		static Impl* s_impl;

		Impl()
		{
			assert(!s_impl && "GameContext is a singleton");
			s_impl = this;
		}

		~Impl()
		{
			s_impl = nullptr;
		}
	};
	
	std::unique_ptr<Impl> pImpl;

public:
	GameContext() noexcept(false)
		: pImpl(std::make_unique<Impl>())
	{
	}

public:
	template<typename Component, typename... Args>
	static Component& Register(Args&& ... args)
	{
		return Impl::s_impl->context.Register<Component, Args...>(std::forward<Args>(args)...);
	}

	template<typename Component>
	static bool Has()
	{
		return Impl::s_impl->context.Has<Component>();
	}

	template<typename Component>
	static void Remove()
	{
		Impl::s_impl->context.Remove<Component>();
	}

	template<typename Component>
	static inline Component& Get()
	{
		return Impl::s_impl->context.Get<Component>();
	}
};