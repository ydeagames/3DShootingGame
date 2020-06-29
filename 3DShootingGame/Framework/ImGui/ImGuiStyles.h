// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-21 07:01:31 +0900
// Modified: 2019-08-05 05:14:05 +0900

#pragma once

namespace ImGuiStyles
{
	inline ImVec4 color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f); }
	inline ImVec4 color(uint8_t r, uint8_t g, uint8_t b) { return color(r, g, b, 0xff); }
	inline ImVec4 color(ImVec4 rgb, uint8_t a) { return ImVec4(rgb.x, rgb.y, rgb.z, a / 255.f); }

	void darkTheme(ImGuiStyle& style);
	void cyberTheme(ImGuiStyle& style);
	void darkrichTheme(ImGuiStyle& style);
}

