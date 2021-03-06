// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2020-02-08 23:19:27 +0900

#include "pch.h"
#include "Input.h"
#include "Framework/ECS/GameContext.h"
#include "Framework/Context/WindowHandler.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

DirectX::Keyboard::KeyboardStateTracker Input::m_keyboardTracker;
DirectX::Mouse::ButtonStateTracker Input::m_mouseTracker;

void Input::Update()
{
	m_keyboardTracker.Update(Keyboard::Get().GetState());
	m_mouseTracker.Update(Mouse::Get().GetState());
}

DirectX::Mouse::ButtonStateTracker::ButtonState Input::GetMouseButtonState(Buttons button)
{
	switch (button)
	{
	case Buttons::MouseLeft:
		return m_mouseTracker.leftButton;
	case Buttons::MouseRight:
		return m_mouseTracker.rightButton;
	case Buttons::MouseMiddle:
		return m_mouseTracker.middleButton;
	case Buttons::MouseXButton1:
		return m_mouseTracker.xButton1;
	case Buttons::MouseXButton2:
		return m_mouseTracker.xButton2;
	default:
		return Mouse::ButtonStateTracker::ButtonState::UP;
	}
}

bool Input::GetMouseButton(Buttons button)
{
	auto state = GetMouseButtonState(button);
	return state == Mouse::ButtonStateTracker::ButtonState::HELD
		|| state == Mouse::ButtonStateTracker::ButtonState::PRESSED;
}

bool Input::GetMouseButtonDown(Buttons button)
{
	auto state = GetMouseButtonState(button);
	return state == Mouse::ButtonStateTracker::ButtonState::PRESSED;
}

bool Input::GetMouseButtonUp(Buttons button)
{
	auto state = GetMouseButtonState(button);
	return state == Mouse::ButtonStateTracker::ButtonState::RELEASED;
}

DirectX::SimpleMath::Vector3 Input::GetMousePosition()
{
	auto state = m_mouseTracker.GetLastState();
	return Vector3(float(state.x), float(state.y), 0.f);
}

void Input::SetMousePosition(const DirectX::SimpleMath::Vector3& pos)
{
	POINT p, p0;
	p = p0 = POINT{ LONG(pos.x), LONG(pos.y) };
	if (!ClientToScreen(GameContext::Get<WindowHandler>().GetHandle(), &p))
		return;
	SetCursorPos(int(p.x), int(p.y));
	Mouse::ProcessMessage(WM_MOUSEMOVE, 1, MAKELPARAM(p0.x, p0.y));
	Update();
}

DirectX::SimpleMath::Vector2 Input::GetMouseScrollDelta()
{
	auto state = m_mouseTracker.GetLastState();
	return Vector2(0.f, float(state.scrollWheelValue));
}

DirectX::Mouse::State Input::GetMouseState()
{
	return m_mouseTracker.GetLastState();
}

DirectX::Mouse::ButtonStateTracker& Input::GetMouseTracker()
{
	return m_mouseTracker;
}

void Input::SetMouseMode(DirectX::Mouse::Mode mode)
{
	Mouse::Get().SetMode(mode);
	Update();
}

DirectX::Mouse::Mode Input::GetMouseMode()
{
	return GetMouseState().positionMode;
}

bool Input::GetKey(DirectX::Keyboard::Keys button)
{
	return m_keyboardTracker.lastState.IsKeyDown(button);
}

bool Input::GetKeyUp(DirectX::Keyboard::Keys button)
{
	return m_keyboardTracker.IsKeyReleased(button);
}

bool Input::GetKeyDown(DirectX::Keyboard::Keys button)
{
	return m_keyboardTracker.IsKeyPressed(button);
}

bool Input::GetAnyKey()
{
	auto currPtr = reinterpret_cast<const uint32_t*>(&m_keyboardTracker.lastState);
	for (size_t j = 0; j < (256 / 32); ++j)
	{
		if (*currPtr != 0)
			return true;
		++currPtr;
	}
	return false;
}

bool Input::GetAnyKeyDown()
{
	auto currPtr = reinterpret_cast<const uint32_t*>(&m_keyboardTracker.pressed);
	for (size_t j = 0; j < (256 / 32); ++j)
	{
		if (*currPtr != 0)
			return true;
		++currPtr;
	}
	return false;
}

DirectX::Keyboard::State Input::GetKeyboardState()
{
	return m_keyboardTracker.GetLastState();
}

DirectX::Keyboard::KeyboardStateTracker& Input::GetKeyboardTracker()
{
	return m_keyboardTracker;
}


