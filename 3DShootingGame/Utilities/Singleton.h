#pragma once

template <class T>
class Singleton
{
public:
	static T& GetInstance()
	{
		static typename T::singleton_pointer_type s_singleton = T::CreateInstance();

		return GetReference(s_singleton);
	}

private:
	typedef std::unique_ptr<T> singleton_pointer_type;

	inline static singleton_pointer_type CreateInstance() { return std::unique_ptr<T>(new T()); }

	inline static T& GetReference(const singleton_pointer_type& ptr) { return *ptr; }

protected:
	Singleton() {}

private:
	Singleton(const Singleton &) = delete;
	Singleton& operator=(const Singleton &) = delete;
	Singleton(Singleton &&) = delete;
	Singleton& operator=(Singleton &&) = delete;
};
