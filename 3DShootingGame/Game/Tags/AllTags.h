// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 16:43:36 +0900
// Modified: 2019-12-16 11:08:16 +0900

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

