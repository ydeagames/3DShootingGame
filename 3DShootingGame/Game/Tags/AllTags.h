#pragma once
#include <Framework/ECS/Tag.h>

namespace Tag
{
	DEFINE_TAG(PlayerTag);
}

namespace BuildSettings
{
	using namespace Tag;
	using Tags = std::tuple<
		PlayerTag
	>;
}