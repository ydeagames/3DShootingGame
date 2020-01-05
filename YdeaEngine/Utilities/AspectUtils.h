#pragma once

namespace AspectUtils
{
	DirectX::SimpleMath::Vector2 Raw(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 Max(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 Width(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 Height(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 Inner(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 Outer(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 WidthLimit(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 HeightLimit(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
	DirectX::SimpleMath::Vector2 Limit(DirectX::SimpleMath::Vector2 size, DirectX::SimpleMath::Vector2 maxsize);
}