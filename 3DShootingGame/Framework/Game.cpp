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
	// ウィンドウ
	m_window = window;

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
	// セーブ
	m_saveHandler = std::make_unique<SaveHandler>(L"Saves/");
	// ポーズ
	m_pauseHandler = std::make_unique<PauseHandler>();
	// 物理
	m_physics = std::make_unique<PhysXManager>();
	m_physics->Initialize(*this);
	// GUI
	m_imgui = std::make_unique<ImGuiManager>();
	m_imgui->Initialize(*this);
	//struct GuiWindow : public ISceneBuilder
	//{
	//	bool show_demo_window = true;
	//	bool show_another_window = false;
	//	float f = 0.0f;
	//	int counter = 0;
	//	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//	void Build(GameContext& context)
	//	{
	//		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//		if (show_demo_window)
	//			ImGui::ShowDemoWindow(&show_demo_window);

	//		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	//		{
	//			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//			ImGui::Checkbox("Another Window", &show_another_window);

	//			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//			ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color

	//			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//				counter++;
	//			ImGui::SameLine();
	//			ImGui::Text("counter = %d", counter);

	//			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//			ImGui::End();
	//		}

	//		// 3. Show another simple window.
	//		if (show_another_window)
	//		{
	//			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//			ImGui::Text("Hello from another window!");
	//			if (ImGui::Button("Close Me"))
	//				show_another_window = false;
	//			ImGui::End();
	//		}
	//	}
	//};
	//m_imgui->RegisterWindow(std::make_shared<GuiWindow>());

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
	m_imgui->Finalize(*this);

	// 物理
	m_physics->Finalize(*this);

	// 破棄
	GetSceneManager().GetSceneView().Finalize(*this);
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
	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		Input::GetMouseTracker().Reset();
	if (io.WantCaptureKeyboard)
		Input::GetKeyboardTracker().Reset();

	// PhysX描画モード
	if (Input::GetKeyDown(Keyboard::Keys::F3))
	{
		switch (m_physics->debugMode)
		{
		case PhysXManager::IngamePvdMode::Game:				m_physics->debugMode = PhysXManager::IngamePvdMode::GameCollision;	break;
		case PhysXManager::IngamePvdMode::GameCollision:	m_physics->debugMode = PhysXManager::IngamePvdMode::Collision;		break;
		default:											m_physics->debugMode = PhysXManager::IngamePvdMode::Game;			break;
		}
	}
	// 物理
	m_physics->Update(*this);
	// GUI
	m_imgui->Update(*this);
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
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

	// クリア
    Clear();

	// ここから描画
    m_deviceResources->PIXBeginEvent(L"Render");

    // TODO: Add your rendering code here.
	if (m_physics->debugMode & PhysXManager::IngamePvdMode::Collision)
	{
		// 物理
		m_physics->Render(*this);
	}
	if (m_physics->debugMode & PhysXManager::IngamePvdMode::Game)
	{
		GetSceneManager().GetSceneView().Render(*this);
	}

	// GUI
	m_imgui->Render(*this);

	// FPS
	m_fps.update();
	if (m_fps.hasFPSChanged())
		SetWindowTextW(GetWindowHandle(), (BuildSettings::GAME_TITLE + L" - FPS: " + std::to_wstring(static_cast<int>(m_fps.getFPS()))).c_str());

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
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
