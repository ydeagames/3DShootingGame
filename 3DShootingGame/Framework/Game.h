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

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify, public GameContext
{
public:

    Game() noexcept(false);

    // Initialization and management
    void Initialize(HWND window, int width, int height);

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
    std::unique_ptr<DX::DeviceResources>	m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer							m_timer;

	// カメラオブジェクト
    GameCamera								m_camera;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates>	m_state;
	
	// エフェクトファクトリー
	std::unique_ptr<DirectX::EffectFactory>	m_effectFactory;

	// シーンマネージャー
	std::unique_ptr<SceneManager>			m_sceneManager;

	// マウス
	std::unique_ptr<DirectX::Mouse>			m_pMouse;

	// キーボード
	std::unique_ptr<DirectX::Keyboard>		m_pKeyboard;

	// 物理
	std::unique_ptr<PhysXManager>			m_physics;

public:
	// DeviceResource取得
	DX::DeviceResources& GetDR()
	{
		return *m_deviceResources;
	}

	// タイマー取得
	DX::StepTimer& GetTimer()
	{
		return m_timer;
	}

	// カメラ取得
	GameCamera& GetCamera()
	{
		return m_camera;
	}

	// コモンステート
	DirectX::CommonStates& GetStates()
	{
		return *m_state;
	}

	// コモンステート
	DirectX::EffectFactory& GetEffectFactory()
	{
		return *m_effectFactory;
	}

	// シーン
	Scene& GetScene()
	{
		return *m_sceneManager->GetActiveScene().scene;
	}

	// オブジェクト追加
	GameContext& operator<<(const std::shared_ptr<GameObject>& obj)
	{
		GetScene().Add(obj);
		return *this;
	}

	// シーン
	SceneManager& GetSceneManager()
	{
		return *m_sceneManager;
	}

	// 物理マネージャ
	PhysXManager& GetPhysics()
	{
		return *m_physics;
	}
};