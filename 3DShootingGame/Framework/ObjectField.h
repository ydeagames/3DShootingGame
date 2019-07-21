#pragma once

template<typename T>
class ObjectHolder
{
private:
	std::shared_ptr<T> m_object;

	ObjectHolder(const std::shared_ptr<T>& object)
		: m_object(object) {}

public:
	constexpr ObjectHolder() noexcept {};
	constexpr ObjectHolder(nullptr_t) noexcept {};
	ObjectHolder(const ObjectHolder&) = delete;
	ObjectHolder& operator=(const ObjectHolder&) = delete;
	ObjectHolder(ObjectHolder&&) noexcept = default;
	ObjectHolder& operator=(ObjectHolder&&) noexcept = default;
	ObjectHolder& operator=(nullptr_t) noexcept {}

public:
	T* operator->() const noexcept { return m_object.get(); }
	explicit operator bool() const noexcept { return m_object; }
	std::weak_ptr<T> GetWeakPtr() const { return m_object; }

	template<typename U = T, typename... Args>
	static ObjectHolder<U> Create(Args&&... args)
	{
		return ObjectHolder<U>(std::make_shared<U>(std::forward(args)...));
	}

	template<typename U = T>
	static ObjectHolder<U> CreateFromUniqueSharedPtr(std::shared_ptr<U>&& ptr)
	{
		return ObjectHolder<U>(std::move(ptr));
	}
};

template<typename T>
class ObjectField
{
private:
	std::weak_ptr<T> m_object;

public:
	ObjectField(const ObjectHolder& object)
		: m_object(object.GetWeakPtr()) {}

	T* operator->() const noexcept
	{
		if (auto p = m_object.lock())
			return m_object.get();std::unique_ptr
		return nullptr;
	}
	explicit operator bool() const noexcept { return !m_object.expired(); }
};
