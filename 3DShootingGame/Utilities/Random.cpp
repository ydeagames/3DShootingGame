#include "pch.h"
#include "Random.h"

namespace Random
{
	std::random_device rd;
	std::mt19937 mt(rd());

	// 0.0≦x≦1.0 の小数ランダム生成
	float Value()
	{
		return static_cast<float>(Rand(RAND_MAX)) / RAND_MAX;
	}

	// 0.0≦x≦max の整数ランダム生成
	int Rand(int max)
	{
		return mt() % (max + 1);
	}

	// 0.0≦x≦max の小数ランダム生成
	float Rand(float max)
	{
		return Value() * max;
	}

	// min≦x≦max の整数ランダム生成
	int Range(int min, int max)
	{
		if (max < min)
			std::swap(min, max);
		return Rand(max - min) + min;
	}

	// min≦x≦max の小数ランダム生成
	float Range(float min, float max)
	{
		if (max < min)
			std::swap(min, max);
		return Rand(max - min) + min;
	}
}
