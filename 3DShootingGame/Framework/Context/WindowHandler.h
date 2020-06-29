// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-21 19:44:18 +0900
// Modified: 2019-12-02 18:33:26 +0900

#pragma once

class WindowHandler
{
private:
	DX::DeviceResources* m_dr;
	HWND m_hwnd;

public:
	WindowHandler(DX::DeviceResources* dr, HWND hwnd)
		: m_dr(dr)
		, m_hwnd(hwnd)
	{}

	HWND GetHandle() { return m_hwnd; }

	DirectX::SimpleMath::Vector2 GetSize()
	{
		RECT size = m_dr->GetOutputSize();
		return DirectX::SimpleMath::Vector2(float(size.right), float(size.bottom));
	}

	float GetAspectRatio()
	{
		auto size = GetSize();
		return float(size.x) / float(size.y);
	}
};

