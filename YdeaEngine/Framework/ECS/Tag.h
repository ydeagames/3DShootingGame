#pragma once

#define DEFINE_TAG(name) \
class name \
{ \
public: \
	static constexpr const char* Identifier = #name; \
 \
public: \
	template<class Archive> \
	void serialize(Archive& archive) \
	{ \
	} \
}