// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-15 16:43:36 +0900
// Modified: 2019-12-15 16:43:36 +0900

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

