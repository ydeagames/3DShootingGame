#include "pch.h"
#include "Menu.h"
#include <Common/DeviceResources.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/PauseHandler.h>
#include <Framework/Context/SceneManager.h>
#include <Utilities/Input.h>
#include <Framework/ExitHandler.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Menu::Update()
{
	if (Input::GetKeyDown(Keyboard::Keys::Escape))
		GameContext::Get<PauseHandler>().SetPaused(false);
}

void Menu::RenderGui(GameCamera& camera)
{
	ImGui::SetNextWindowPosCenter();
	ImGui::SetNextWindowSize(ImVec2(230, 300));
	ImGui::Begin(u8"�X�����O�q�[���[�Y", nullptr/*, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize*/);
	if (ImGui::Button(u8"�Q�[���X�^�[�g", ImVec2(200, 50)))
	{
		GameContext::Get<SceneManager>().LoadScene(SceneInfo{ "PlayScene", "PlayScene.scene.json" }); // LoadSceneWithTransition(L"PlayScene", SceneTransitions::CreateRuleTransition(3));
	}
	if (ImGui::Button(u8"�I�v�V����", ImVec2(200, 50)))
	{

	}
	if (ImGui::Button(u8"�N���W�b�g", ImVec2(200, 50)))
	{
		showCredit = true;
	}
	if (ImGui::Button(u8"�I��", ImVec2(200, 50)))
	{
		ExitGame();
	}
	ImGui::End();

	if (showCredit)
	{
		ImGui::SetNextWindowPosCenter();
		ImGui::SetNextWindowSize(ImVec2(700, 200));
		ImGui::Begin(u8"�N���W�b�g", nullptr, ImGuiFocusedFlags_None);
		ImGui::Text(u8"�X�����O�q�[���[�Y - (c) 2019 YdeaGames");
		ImGui::Text(u8"PhysX - Copyright (c) 2019 NVIDIA Corporation. All rights reserved.");
		ImGui::Text(u8"ImGui - Copyright (c) 2014-2019 Omar Cornut");
		if (ImGui::Button(u8"����", ImVec2(680, 30)))
		{
			showCredit = false;
		}
		ImGui::End();
	}
}