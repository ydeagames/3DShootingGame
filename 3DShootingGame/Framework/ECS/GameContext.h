#pragma once
#include "Context.h"

class GameContext final
{
private:
	class Impl
	{
	public:
		Context context;

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