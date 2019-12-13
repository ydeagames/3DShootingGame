#include "pch.h"
#include "MyGame.h"
#include <Framework/ECS/Serialize.h>
#include <Framework/ImGui/ImGuiManager.h>
#include <Framework/ImGui/Widgets.h>
#include <Framework/ECS/Component.h>
#include <Framework/ECS/Events.h>
#include <Framework/ECS/Scene.h>
#include <Framework/Components/AllComponents.h>
#include <Framework/Context/SceneManager.h>
#include <Framework/PhysX/PhysXManager.h>
#include <Framework/Context/SaveHandler.h>
#include <Framework/Context/WindowHandler.h>
#include <Utilities/FPS.h>
#include <Utilities/Input.h>
#include <Game/BuildSettings.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

int MyGame::Bench()
{
	//constexpr long count = 100'000'000L;
	//{
	//	std::cout << "Performance Test" << std::endl;
	//	auto t1 = std::chrono::high_resolution_clock::now();
	//	for (long i = 0; i < count; i++)
	//	{
	//		auto& camera = GameContext::Get<Camera>();
	//	}
	//	auto t2 = std::chrono::high_resolution_clock::now();
	//	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//	std::cout << "Finish in " << duration / 1'000'000.f << " (" << duration / (float)count << "μs)" << std::endl;
	//}
	return 0;
}

MyGame::MyGame()
{
	// Events
	ECS::AllComponents::InitializeEvents();
	// Widgets
	GameContext::Register<Widgets::EntityEditorState>();
	GameContext::Register<MM::ImGuiEntityEditor<entt::registry>>();
	ECS::AllComponents::InitializeEditorComponents(GameContext::Get<MM::ImGuiEntityEditor<entt::registry>>());

	// FPS
	GameContext::Register<FPS>(GameContext::Get<DX::StepTimer>());
	// コモンステートを作成する
	GameContext::Register<CommonStates>(GameContext::Get<DX::DeviceResources>().GetD3DDevice());
	// EffectFactoryオブジェクトを生成する
	GameContext::Register<EffectFactory>(GameContext::Get<DX::DeviceResources>().GetD3DDevice());
	// テクスチャの読み込みパス指定
	GameContext::Get<EffectFactory>().SetDirectory(L"Resources/Models");
	// セーブ
	GameContext::Register<SaveHandler>(L"Saves/");
	// 物理
	GameContext::Register<PhysXManager>();
	// シーン
	GameContext::Register<SceneManager>().LoadScene(SceneInfo::CreateFromName("scene"));

	// Transform
	GameContext::Register<TransformResolver>();
}

MyGame::~MyGame()
{
	GameContext::Remove<TransformResolver>();
	GameContext::Remove<SceneManager>();
	GameContext::Remove<PhysXManager>();
	GameContext::Remove<SaveHandler>();
	GameContext::Remove<EffectFactory>();
	GameContext::Remove<CommonStates>();
	GameContext::Remove<FPS>();
	GameContext::Remove<MM::ImGuiEntityEditor<entt::registry>>();
	GameContext::Remove<Widgets::EntityEditorState>();
}

void MyGame::Update()
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
		auto& physics = GameContext::Get<PhysXManager>();
		switch (physics.debugMode)
		{
		case PhysXManager::IngamePvdMode::Game:				physics.debugMode = PhysXManager::IngamePvdMode::GameCollision;	break;
		case PhysXManager::IngamePvdMode::GameCollision:	physics.debugMode = PhysXManager::IngamePvdMode::Collision;		break;
		default:											physics.debugMode = PhysXManager::IngamePvdMode::Game;			break;
		}
	}

	auto& editorState = GameContext::Get<Widgets::EntityEditorState>();

	// エディター
	if (Input::GetKeyDown(Keyboard::Keys::F4))
		editorState.editorEnabled = !editorState.editorEnabled;

	// マウス相対座標切り替え
	if (Input::GetKeyDown(Keyboard::Keys::F6))
		Mouse::Get().SetMode(Mouse::Get().GetState().positionMode == Mouse::Mode::MODE_ABSOLUTE ? Mouse::Mode::MODE_RELATIVE : Mouse::Mode::MODE_ABSOLUTE);

	// Updateイベント
	GameContext::Get<SceneManager>().ForEachScenes([](auto& scene) { Updatable::Update(scene.registry); });

	// シーン遷移
	GameContext::Get<SceneManager>().Apply();
}

void MyGame::RenderStart()
{
	m_imgui = std::make_unique<ImGuiPtr>();

	GameContext::Get<SceneManager>().ForEachScenes([](auto& scene) { Renderable::RenderStart(scene.registry); });
}

void MyGame::Render(GameCamera& camera)
{
	static int bench = Bench();

	// トランスフォームのキャッシュをクリア
	GameContext::Get<TransformResolver>().ClearCache();

	// 描画イベント
	GameContext::Get<SceneManager>().ForEachScenesInverted([&](auto& scene) { Renderable::Render(scene.registry, std::forward<GameCamera>(camera)); });
	
	//auto& physics = Get<PhysXManager>();
	//if (physics.debugMode & PhysXManager::IngamePvdMode::Game)
	//{
	//	GetSceneManager().GetSceneView().Render(*this);
	//}
	//else
	//{
	//	GetSceneManager().GetActiveScene().Find(L"SceneDirector")->Render(*this);
	//}

	// ImGui
	{
		// ImGuiコンテキスト
		auto& imgui = GameContext::Get<ImGuiManager>();

		// ImGui描画開始
		imgui.Begin();

		// GUI描画イベント
		GameContext::Get<SceneManager>().ForEachScenesInverted([&](auto& scene) { Renderable::RenderGui(scene.registry, std::forward<GameCamera>(camera)); });

		// Widgets
		Widgets::AllWidgets::Render(GameContext::Get<SceneManager>().GetActiveScene());

		// ImGui描画終了
		imgui.End();
	}

	// FPS
	auto& fps = GameContext::Get<FPS>();
	fps.update();
	if (fps.hasFPSChanged())
	{
		std::wstringstream sb;
		sb << BuildSettings::GAME_WINDOW_TITLE << L" - FPS: " << static_cast<int>(fps.getFPS());
		SetWindowTextW(GameContext::Get<WindowHandler>().GetHandle(), sb.str().c_str());
	}
}

class MyGame::ImGuiPtr
{
public:
	ImGuiPtr()
	{
		// ImGuiコンテキスト
		auto& imgui = GameContext::Register<ImGuiManager>();

		// ImGui初期化
		imgui.RenderStart();
	}

	~ImGuiPtr()
	{
		// ImGuiコンテキスト
		auto& imgui = GameContext::Get<ImGuiManager>();

		// ImGuiファイナライズ
		imgui.RenderFinalize();

		// ImGui削除
		GameContext::Remove<ImGuiManager>();
	}
};
