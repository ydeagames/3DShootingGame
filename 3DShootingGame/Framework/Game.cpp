//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <Game/BuildSettings.h>
#include <Utilities/Input.h>

#include <WICTextureLoader.h>

#include "ExitHandler.h"

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

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// オブジェクトタイム
float Object::objectTime = 0;

Game::Game() noexcept(false)
{
	// StepTimer
	Register(std::make_unique<DX::StepTimer>());
	Object::objectTime = float(Get<DX::StepTimer>().GetTotalSeconds());

	// デバイスリソース初期化
	Register(std::make_unique<DX::DeviceResources>());
	m_deviceResources = &Get<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);

	// シーンマネージャー
	Register(std::make_unique<SceneManager>());
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	// マウスの作成
	Register(std::make_unique<Mouse>());
	Get<Mouse>().SetWindow(window);

	// キーボードの作成
	Register(std::make_unique<Keyboard>());

	// 設定
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    m_deviceResources->CreateWindowSizeDependentResources();

	{
		// FPS
		Register(std::make_unique<FPS>(Get<DX::StepTimer>()));
		// ウィンドウ
		Register(std::make_unique<WindowHandler>(m_deviceResources, window));
		// カメラ
		Register(std::make_unique<GameCamera>());
		// コモンステートを作成する
		Register(std::make_unique<CommonStates>(m_deviceResources->GetD3DDevice()));
		// EffectFactoryオブジェクトを生成する
		Register(std::make_unique<EffectFactory>(m_deviceResources->GetD3DDevice()));
		// テクスチャの読み込みパス指定
		Get<EffectFactory>().SetDirectory(L"Resources/Models");
		// セーブ
		Register(std::make_unique<SaveHandler>(L"Saves/"));
		// ポーズ
		Register(std::make_unique<PauseHandler>());
		// 物理
		Register(std::make_unique<PhysXManager>());
		Get<PhysXManager>().Initialize(*this);
		// GUI
		Register(std::make_unique<ImGuiManager>());
		Get<ImGuiManager>().Initialize(*this);
	}

	CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();

	// 作成
	GetSceneManager().Register<BuildSettings>();
	GetSceneManager().LoadScene(L"BuildSettings");

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

void Game::Finalize()
{
	// GUI
	Get<ImGuiManager>().Finalize(*this);

	// 物理
	Get<PhysXManager>().Finalize(*this);

	// 破棄
	GetSceneManager().GetSceneView().Finalize(*this);
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	// 毎チック処理
	auto& timer = Get<DX::StepTimer>();
	Object::objectTime = float(timer.GetTotalSeconds());
	timer.Tick([&]()
    {
        Update(timer);
    });

	// 描画
    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	// インプット更新
	Input::Update();
	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		Input::GetMouseTracker().Reset();
	if (io.WantCaptureKeyboard)
		Input::GetKeyboardTracker().Reset();

	// PhysX描画モード
	if (Input::GetKeyDown(Keyboard::Keys::F3))
	{
		auto& physics = Get<PhysXManager>();
		switch (physics.debugMode)
		{
		case PhysXManager::IngamePvdMode::Game:				physics.debugMode = PhysXManager::IngamePvdMode::GameCollision;	break;
		case PhysXManager::IngamePvdMode::GameCollision:	physics.debugMode = PhysXManager::IngamePvdMode::Collision;		break;
		default:											physics.debugMode = PhysXManager::IngamePvdMode::Game;			break;
		}
	}
	// 物理
	Get<PhysXManager>().Update(*this);
	// GUI
	Get<ImGuiManager>().Update(*this);
	// シーン処理
	GetSceneManager().ProcessScene(*this);
	// 更新
	GetSceneManager().GetSceneView().Update(*this);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (Get<DX::StepTimer>().GetFrameCount() == 0)
    {
        return;
    }

	// クリア
    Clear();

	// ここから描画
    m_deviceResources->PIXBeginEvent(L"Render");

    // TODO: Add your rendering code here.
	auto& physics = Get<PhysXManager>();
	if (physics.debugMode & PhysXManager::IngamePvdMode::Collision)
	{
		// 物理
		physics.Render(*this);
	}
	if (physics.debugMode & PhysXManager::IngamePvdMode::Game)
	{
		GetSceneManager().GetSceneView().Render(*this);
	}

	// GUI
	Get<ImGuiManager>().Render(*this);

	// FPS
	auto& fps = Get<FPS>();
	fps.update();
	if (fps.hasFPSChanged())
		SetWindowTextW(Get<WindowHandler>().GetHandle(), (BuildSettings::GAME_TITLE + L" - FPS: " + std::to_wstring(static_cast<int>(fps.getFPS()))).c_str());

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
	Input::SetMouseMode(GetScene().mouseMode);
	GetPauseHandler().SetPaused(*this, false);
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
	Input::SetMouseMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
	GetPauseHandler().SetPaused(*this, true);
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	Get<DX::StepTimer>().ResetElapsedTime();

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
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

	// ウインドウサイズからアスペクト比を算出する
	auto& window = Get<WindowHandler>();
	float aspectRatio = window.GetAspectRatio();
	auto size = window.GetSize();
	// 画角を設定
	float fovAngleY = XMConvertToRadians(70.0f);
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
