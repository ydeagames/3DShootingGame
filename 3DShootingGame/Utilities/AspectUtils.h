// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-07-19 15:29:42 +0900

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

