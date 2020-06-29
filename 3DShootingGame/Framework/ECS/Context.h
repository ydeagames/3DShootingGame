// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 05:03:34 +0900
// Modified: 2019-12-15 16:43:36 +0900

#pragma once

class Context
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
		assert(Has<Component>() && "Not Registered");
		holder<Component>().data = nullptr;
	}

	template<typename Component>
	inline Component& Get()
	{
		return *holder<Component>().data;
	}
};


