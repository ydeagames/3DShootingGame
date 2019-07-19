#include "pch.h"
#include "AspectUtils.h"

namespace AspectUtils
{
	DirectX::SimpleMath::Vector2 Raw(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		return size;
	}

	DirectX::SimpleMath::Vector2 Max(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		return maxsize;
	}

	DirectX::SimpleMath::Vector2 Width(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		return{ maxsize.x, size.y*maxsize.x / size.x };
	}

	DirectX::SimpleMath::Vector2 Height(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		return{ size.x*maxsize.y / size.y, maxsize.y };
	}

	DirectX::SimpleMath::Vector2 Inner(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		if (size.x < 0)
			maxsize.x *= -1;
		if (size.y < 0)
			maxsize.y *= -1;
		boolean b = size.x / maxsize.x > size.y / maxsize.y;
		return{ b ? maxsize.x : size.x*maxsize.y / size.y, b ? size.y*maxsize.x / size.x : maxsize.y };
	}

	DirectX::SimpleMath::Vector2 Outer(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		if (size.x < 0)
			maxsize.x *= -1;
		if (size.y < 0)
			maxsize.y *= -1;
		boolean b = size.x / maxsize.x < size.y / maxsize.y;
		return{ b ? maxsize.x : size.x*maxsize.y / size.y, b ? size.y*maxsize.x / size.x : maxsize.y };
	}

	DirectX::SimpleMath::Vector2 WidthLimit(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		if (size.x < maxsize.x)
			return size;
		else
			return{ maxsize.x, maxsize.x*size.y / size.x };
	}

	DirectX::SimpleMath::Vector2 HeightLimit(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		if (size.y < maxsize.y)
			return size;
		else
			return{ maxsize.y*size.x / size.y, maxsize.y };
	}

	DirectX::SimpleMath::Vector2 Limit(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize) {
		if (size.x > size.y)
			if (size.x < maxsize.x)
				return size;
			else
				return{ maxsize.x, maxsize.x*size.y / size.x };
		else if (size.y < maxsize.y)
			return size;
		else
			return{ maxsize.y*size.x / size.y, maxsize.y };
	}
}