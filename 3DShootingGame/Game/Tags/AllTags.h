#pragma once
#include <Framework/ECS/Tag.h>

namespace Tag
{
	DEFINE_TAG(TagA);
	DEFINE_TAG(TagB);
	DEFINE_TAG(TagC);
	DEFINE_TAG(TagD);
}

namespace BuildSettings
{
	using namespace Tag;
	using Tags = std::tuple<
		TagA,
		TagB,
		TagC,
		TagD
	>;
}