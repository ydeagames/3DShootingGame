//
// Game.h
//

#pragma once

#include <Common/DeviceResources.h>
#include <Common/StepTimer.h>
#include "MyGame.h"
#include <Framework/ECS/GameContext.h>

class GameCamera;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

	Game() noexcept(false);
	~Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);
	void Finalize();

	// Basic game loop
	void Tick();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);

	// Properties
	void GetDefaultSize( int& width, int& height ) const;

private:

	void Update(DX::StepTimer const& timer);
	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// Device resources.
	DX::DeviceResources*					m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer*							m_timer;

	// Mouse
	std::unique_ptr<DirectX::Mouse>			m_mouse;
	// Keyboard
	std::unique_ptr<DirectX::Keyboard>		m_keyboard;

	// Context
	GameContext								m_context;
	// Camera
	std::unique_ptr<GameCamera>				m_mainCamera;
	// Game
	std::unique_ptr<MyGame>					m_myGame;
};
