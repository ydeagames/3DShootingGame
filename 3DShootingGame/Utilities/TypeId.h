#pragma once
#include <xhash>

class type_id_t;
template<>
struct std::hash<type_id_t>;

// https://codereview.stackexchange.com/questions/48594/unique-type-id-no-rtti
class type_id_t
{
	using sig = type_id_t();

	sig* id;
	type_id_t(sig* id) : id{ id } {}

public:
	template<typename T>
	friend type_id_t type_id();

	friend struct std::hash<type_id_t>;

	bool operator==(type_id_t o) const { return id == o.id; }
	bool operator!=(type_id_t o) const { return id != o.id; }
};

template<typename T>
type_id_t type_id() { return &type_id<T>; }

namespace std {
	template <>
	struct hash<type_id_t> {
		size_t operator()(const type_id_t& id) const {
			return reinterpret_cast<size_t>(id.id);
		}
	};
}