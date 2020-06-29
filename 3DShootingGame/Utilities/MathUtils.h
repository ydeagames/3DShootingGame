// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-07-19 15:29:42 +0900

#pragma once

namespace MathUtils
{
	static constexpr float FloatEpsilon = 10e-6f;

	// 範囲内に制限
	template<class T> T Clamp(T x, T min, T max)
	{
		return std::min(std::max(x, min), max);
	}

	// 0～w の整数ループ
	int Loop(int x, int w);

	// 0～w の小数ループ
	float Loop(float x, float w);

	// min～max の整数ループ
	int Loop(int x, int min, int max);

	// min～max の小数ループ
	float Loop(float x, float min, float max);

	// 浮動小数点数型の誤差を考慮に入れた比較
	bool FloatEquals(float a, float b);

	// 0～maxの範囲でxは何%の位置にいるのか
	// ※ 範囲外は範囲内に収められます
	float Percentage(float x, float max);

	// min～maxの範囲でxは何%の位置にいるのか
	// ※ 範囲外は範囲内に収められます
	float Percentage(float x, float min, float max);

	// 0～maxの範囲でのpercent%の位置
	float Lerp(float percent, float max);

	// min～maxの範囲でのpercent%の位置
	float Lerp(float percent, float min, float max);
}


