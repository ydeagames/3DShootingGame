// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-07-19 15:29:42 +0900

#pragma once

namespace Random
{
	// 0.0≦x≦1.0 の小数ランダム生成
	float Value();

	// 0.0≦x≦max の整数ランダム生成
	int Rand(int max);

	// 0.0≦x≦max の小数ランダム生成
	float Rand(float max);

	// min≦x≦max の整数ランダム生成
	int Range(int min, int max);

	// min≦x≦max の小数ランダム生成
	float Range(float min, float max);
}


