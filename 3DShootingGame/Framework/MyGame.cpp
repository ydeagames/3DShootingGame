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
	//	std::cout << "Finish in " << duration / 1'000'000.f << " (" << duration / (float)count << "��s)" << std::endl;
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
		// ImGui�R���e�L�X�g
		auto imgui = GameContext::Register<ImGuiManager>();

		// ImGui������
		imgui.RenderInitialize();
	}

	~ImGuiPtr()
	{
		// ImGui�R���e�L�X�g
		auto& imgui = GameContext::Get<ImGuiManager>();

		// ImGui�t�@�C�i���C�Y
		imgui.RenderFinalize();

		// ImGui�폜
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

	// �g�����X�t�H�[���̃L���b�V�����N���A
	GameContext::Get<TransformResolver>().ClearCache();

	// �`��C�x���g
	Renderable::Render(m_scene, std::forward<GameCamera>(camera));

	// ImGui
	{
		// ImGui�R���e�L�X�g
		auto& imgui = GameContext::Get<ImGuiManager>();

		// ImGui�`��J�n
		imgui.Begin();

		// GUI�`��C�x���g
		Renderable::RenderGui(m_scene, std::forward<GameCamera>(camera));

		// Widgets
		Widgets::AllWidgets::Render(m_scene);

		// ImGui�`��I��
		imgui.End();
	}
}