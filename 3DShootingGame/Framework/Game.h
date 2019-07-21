//
// Game.h
//

#pragma once

#include <Common/DeviceResources.h>
#include <Common/StepTimer.h>

#include <Mouse.h>
#include <Keyboard.h>

#include <SpriteBatch.h>
#include <SimpleMath.h>

#include "GameObject.h"
#include "GameContext.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameCamera.h"
#include <Framework/PhysX/PhysXManager.h>
#include "ImGuiManager.h"
#include "SaveHandler.h"
#include "PauseHandler.h"
#include <Utilities/FPS.h>
#include "WindowHandler.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify, public GameContext
{
public:

    Game() noexcept(false);

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

public:
	// シーン
	Scene& GetScene() { return GetSceneManager().GetActiveScene(); }

	// オブジェクト追加
	GameContext& operator<<(const std::shared_ptr<GameObject>& obj) { GetScene().Add(obj); return *this; }
};