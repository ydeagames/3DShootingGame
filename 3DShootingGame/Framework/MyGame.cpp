#include "pch.h"
#include "MyGame.h"
#include <Framework/ECS/Serialize.h>
#include <Framework/ImGui/ImGuiManager.h>
#include <Framework/ImGui/Widgets.h>
#include <Framework/Components/Components.h>
#include <Framework/Components/AllComponents.h>
#include <Framework/Context/SceneManager.h>

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
	Components::InitializeEvents();

	GameContext::Register<SceneManager>(m_scene);

	m_scene.info.name = "scene";
	m_scene.info.location = m_scene.info.name + ".json";
	m_scene.Load();

	// Widgets
	Widgets::AllWidgets::Initialize(m_scene);

	GameContext::Register<TransformResolver>();
}

MyGame::~MyGame()
{
	GameContext::Remove<TransformResolver>();
	GameContext::Remove<SceneManager>();
}

void MyGame::Update()
{
	Updatable::Update(m_scene);
}

class MyGame::ImGuiPtr
{
public:
	ImGuiPtr()
	{
		// ImGuiコンテキスト
		auto imgui = GameContext::Register<ImGuiManager>();

		// ImGui初期化
		imgui.RenderInitialize();
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

void MyGame::RenderInitialize()
{
	m_imgui = std::make_unique<ImGuiPtr>();

	Renderable::RenderInitialize(m_scene);
}

void MyGame::Render(GameCamera& camera)
{
	static int bench = Bench();

	// トランスフォームのキャッシュをクリア
	GameContext::Get<TransformResolver>().ClearCache();

	// 描画イベント
	Renderable::Render(m_scene, std::forward<GameCamera>(camera));

	// ImGui
	{
		// ImGuiコンテキスト
		auto& imgui = GameContext::Get<ImGuiManager>();

		// ImGui描画開始
		imgui.Begin();

		// GUI描画イベント
		Renderable::RenderGui(m_scene, std::forward<GameCamera>(camera));

		// Widgets
		Widgets::AllWidgets::Render(m_scene);

		// ImGui描画終了
		imgui.End();
	}
}
