#pragma once
#include "Component.h"

class Event
{
public:
	Event() = default;
	virtual ~Event() = default;
};

//class ChildEvent : public Event
//{
//};

class EventBus : public Component
{
private:
	// Event Dispatcher class from https://stackoverflow.com/a/42350751
	class event_dispatcher
	{
	public:
		template <typename F>
		struct function_traits : public function_traits<decltype(&F::operator())>
		{};

		template <typename T, typename R, typename E>
		struct function_traits<R(T::*)(E) const>
		{
			typedef R(*pointer)(E);
			typedef R return_type;
			typedef E args_type;
			typedef const std::function<R(E)> function;
		};

	private:
		struct function_wrapper
		{
			virtual ~function_wrapper() {}
			virtual const void* get_ptr() const = 0;
		};

		template<typename F>
		class function_wrapper_t : public function_wrapper
		{
		public:
			function_wrapper_t(F&& f) : _function(f) {}
			~function_wrapper_t() {}
			const void* get_ptr() const { return &_function; }

		private:
			typename function_traits<F>::function _function;
		};

		template <typename F>
		std::unique_ptr<function_wrapper> create_wrapper(F f)
		{
			return std::unique_ptr<function_wrapper_t<decltype(f)>>(new function_wrapper_t<decltype(f)>(std::forward<F>(f)));
		}

	private:
		std::unordered_map<std::type_index, std::vector<std::unique_ptr<function_wrapper>>> _list;

	public:
		template<typename F>
		void connect(const std::type_index& name, F f)
		{
			static_assert(std::is_same<void, typename function_traits<F>::return_type>::value,
				"The event handler must be a lambda expression with a return value of type void");

			_list[name].emplace_back(create_wrapper(std::forward<F>(f)));
		}

		template<typename E>
		void dispatch(const std::type_index& name, E e)
		{
			auto& funcs = _list[name];

			for (auto& func : funcs)
			{
				auto& f = *reinterpret_cast<const std::function<void(E)>*>(func->get_ptr());
				f(std::forward<E>(e)); // is this undefined behavior?
			}
		}
	} dispatcher;

public:
	template<class F>
	void Register(F listener)
	{
		//static_assert(std::is_base_of<Event, event_dispatcher::function_traits<F>::args_type>::value,
		//	"The event handler must be a lambda expression with only one argument that inherits from the Event type");

		//auto& a = typeid(Event);
		//auto& b = typeid(ChildEvent);
		//auto& c = typeid(event_dispatcher::function_traits<F>::args_type);
		//auto& d = typeid(F);
		//_RPT0(_CRT_WARN, typeid(event_dispatcher::function_traits<F>::args_type).name());
		//_RPT0(_CRT_WARN, std::to_string(std::is_base_of<Event, ChildEvent>::value).c_str());
		//_RPT0(_CRT_WARN, std::to_string(std::is_base_of<Event, event_dispatcher::function_traits<F>::args_type>::value).c_str());

		dispatcher.connect(typeid(event_dispatcher::function_traits<F>::args_type), listener);
	}

	template<class T>
	void Post(T& eventObj)
	{
		dispatcher.dispatch(typeid(T), eventObj);
	}
};

