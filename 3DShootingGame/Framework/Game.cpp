//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <Game/BuildSettings.h>
#include <Utilities/Input.h>

#include <WICTextureLoader.h>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	// デバイスリソース初期化
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);

	// シーンマネージャー
	m_sceneManager = std::make_unique<SceneManager>();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	// マウスの作成
	m_pMouse = std::make_unique<Mouse>();
	m_pMouse->SetWindow(window);

	// キーボードの作成
	m_pKeyboard = std::make_unique<Keyboard>();

	// 設定
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    m_deviceResources->CreateWindowSizeDependentResources();

	// コモンステートを作成する
	m_state = std::make_unique<CommonStates>(m_deviceResources->GetD3DDevice());
	// EffectFactoryオブジェクトを生成する
	m_effectFactory = std::make_unique<DirectX::EffectFactory>(m_deviceResources->GetD3DDevice());
	// テクスチャの読み込みパス指定
	m_effectFactory->SetDirectory(L"Resources/Models");
	// 物理
	m_physics = std::make_unique<PhysXManager>();
	m_physics->Initialize(*this);

    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	// 毎チック処理
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	// 描画
    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	// インプット更新
	Input::Update();
	// 物理
	m_physics->Update(*this);
	// シーン処理
	GetSceneManager().ProcessScene(*this);
	// 更新
	GetSceneManager().GetActiveScene().scene->Update(*this);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

	// クリア
    Clear();

	// ここから描画
    m_deviceResources->PIXBeginEvent(L"Render");

	// 物理
	m_physics->Render(*this);

    // TODO: Add your rendering code here.
	GetSceneManager().GetActiveScene().scene->Render(*this);

	// ここまで描画
    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    // TODO: Initialize device dependent objects here (independent of window size).

	// 作成
	BuildSettings().Build(*this);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

	// ウインドウサイズからアスペクト比を算出する
	RECT size = m_deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	// 画角を設定
	float fovAngleY = XMConvertToRadians(45.0f);
	// 射影行列を作成する
	GetCamera().projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		fovAngleY,
		aspectRatio,
		0.01f,
		10000.0f
	);
	// ビューポート行列を作成する
	GetCamera().viewport = 
		SimpleMath::Matrix::CreateScale(Vector3(.5f, -.5f, 1.f)) *
		SimpleMath::Matrix::CreateTranslation(Vector3(.5f, .5f, 0.f)) *
		SimpleMath::Matrix::CreateScale(Vector3(float(size.right), float(size.bottom), 1.f));
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

	// 物理
	m_physics->Finalize(*this);

	// 破棄
	GetSceneManager().GetActiveScene().scene->Finalize(*this);
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
