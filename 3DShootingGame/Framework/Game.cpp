//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <Framework/Context/ApplicationHandler.h>
#include <Framework/Context/WindowHandler.h>
#include <Framework/Context/PauseHandler.h>
#include <Framework/Context/GameCamera.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	m_timer = &GameContext::Register<DX::StepTimer>();

    m_deviceResources = &GameContext::Register<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	// Mouse
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	// Keyboard
	m_keyboard = std::make_unique<Keyboard>();

	// 設定
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    m_deviceResources->CreateWindowSizeDependentResources();

	m_mainCamera = std::make_unique<GameCamera>();
	m_myGame = std::make_unique<MyGame>();

	// アプリケーション
	GameContext::Register<ApplicationHandler>();
	// ウィンドウ
	GameContext::Register<WindowHandler>(m_deviceResources, window);
	// ポーズ
	GameContext::Register<PauseHandler>();

	CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();


    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

void Game::Finalize()
{
	GameContext::Remove<PauseHandler>();
	GameContext::Remove<WindowHandler>();
	GameContext::Remove<ApplicationHandler>();

	// 破棄
	m_myGame = nullptr;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	// 毎チック処理
	m_timer->Tick([&]()
    {
        Update(*m_timer);
    });

	// 描画
    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	// 更新
	m_myGame->Update();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer->GetFrameCount() == 0)
    {
        return;
    }

	// クリア
    Clear();

	// ここから描画
    m_deviceResources->PIXBeginEvent(L"Render");

    // TODO: Add your rendering code here.

	m_myGame->Render(*m_mainCamera);

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
    auto shadowMapDepthStencil = m_deviceResources->GetShadowMapDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->ClearDepthStencilView(shadowMapDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
	//Input::SetMouseMode(GetScene().mouseMode);
	//GameContext::Get<PauseHandler>().SetPaused(false);
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
	//Input::SetMouseMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
	GameContext::Get<PauseHandler>().SetPaused(true);
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer->ResetElapsedTime();

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
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;

	m_myGame->RenderStart();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

	// ウインドウサイズからアスペクト比を算出する
	auto& window = GameContext::Get<WindowHandler>();
	float aspectRatio = window.GetAspectRatio();
	auto size = window.GetSize();
	// 画角を設定
	float fovAngleY = XMConvertToRadians(70.0f);
	// 射影行列を作成する
	m_mainCamera->projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		fovAngleY,
		aspectRatio,
		0.01f,
		10000.0f
	);
	// ビューポート行列を作成する
	m_mainCamera->viewport = 
		SimpleMath::Matrix::CreateScale(Vector3(.5f, -.5f, 1.f)) *
		SimpleMath::Matrix::CreateTranslation(Vector3(.5f, .5f, 0.f)) *
		SimpleMath::Matrix::CreateScale(Vector3(size.x, size.y, 1.f));
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
