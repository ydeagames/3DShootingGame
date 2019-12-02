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