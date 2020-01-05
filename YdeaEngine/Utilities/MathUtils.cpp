#include "pch.h"
#include "MathUtils.h"

namespace MathUtils
{
	// 0～w の整数ループ
	int Loop(int x, int w)
	{
		return ((x % w) + w) % w;
	}

	// 0～w の小数ループ
	float Loop(float x, float w)
	{
		return std::fmodf((std::fmodf(x, w) + w), w);
	}

	// min～max の整数ループ
	int Loop(int x, int min, int max)
	{
		if (max < min)
			std::swap(min, max);
		return Loop(x - min, max - min) + min;
	}

	// min～max の小数ループ
	float Loop(float x, float min, float max)
	{
		if (max < min)
			std::swap(min, max);
		return Loop(x - min, max - min) + min;
	}

	// 浮動小数点数型の誤差を考慮に入れた比較
	bool FloatEquals(float a, float b)
	{
		return std::fabsf(a - b) <= FloatEpsilon;
	}

	// 0～maxの範囲でxは何%の位置にいるのか
	// ※ 範囲外は範囲内に収められます
	float Percentage(float x, float max)
	{
		float n = Clamp(x, 0.f, max);
		return n / max;
	}

	// min～maxの範囲でxは何%の位置にいるのか
	// ※ 範囲外は範囲内に収められます
	float Percentage(float x, float min, float max)
	{
		return Percentage(x - min, max - min);
	}

	// 0～maxの範囲でのpercent%の位置
	float Lerp(float percent, float max)
	{
		return max * percent;
	}

	// min～maxの範囲でのpercent%の位置
	float Lerp(float percent, float min, float max)
	{
		return Lerp(percent, max - min) + min;
	}
}
