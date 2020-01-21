#include "pch.h"
#include "Menu.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/PauseHandler.h>
#include <Framework/Context/SceneManager.h>
#include <Utilities/Input.h>
#include <Framework/Context/ApplicationHandler.h>
#include <Framework/SceneManagement/SceneTransitions.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void TitleMenu::Update()
{
	if (Input::GetKeyDown(Keyboard::Keys::Escape))
		GameContext::Get<PauseHandler>().SetPaused(true);
}

void TitleMenu::RenderGui(GameCamera& camera)
{
	ImGui::SetNextWindowPosCenter();
	ImGui::SetNextWindowSize(ImVec2(230, 300));
	ImGui::Begin(u8"スリングヒーローズ", nullptr/*, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize*/);
	if (ImGui::Button(u8"ゲームスタート", ImVec2(200, 50)))
	{
		GameContext::Get<SceneManager>().LoadSceneTransition(SceneInfo::CreateFromName("PlayScene"), SceneTransitions::CreateRuleTransition(3));
	}
	if (ImGui::Button(u8"オプション", ImVec2(200, 50)))
	{

	}
	if (ImGui::Button(u8"クレジット", ImVec2(200, 50)))
	{
		showCredit = true;
	}
	if (ImGui::Button(u8"終了", ImVec2(200, 50)))
	{
		ApplicationHandler::ExitGame();
	}
	ImGui::End();

	if (showCredit)
	{
		ImGui::SetNextWindowPosCenter();
		ImGui::SetNextWindowSize(ImVec2(700, 200));
		ImGui::Begin(u8"クレジット", nullptr, ImGuiFocusedFlags_None);
		ImGui::Text(u8"スリングヒーローズ - (c) 2019 YdeaGames");
		ImGui::Text(u8"PhysX - Copyright (c) 2019 NVIDIA Corporation. All rights reserved.");
		ImGui::Text(u8"ImGui - Copyright (c) 2014-2019 Omar Cornut");
		if (ImGui::Button(u8"閉じる", ImVec2(680, 30)))
		{
			showCredit = false;
		}
		ImGui::End();
	}
}

void PlayMenu::Start()
{
	last = float(GameContext::Get<DX::StepTimer>().GetTotalSeconds());
}

void PlayMenu::Update()
{
	if (Input::GetKeyDown(Keyboard::Keys::Escape))
		GameContext::Get<PauseHandler>().SetPaused(true);
}

void PlayMenu::RenderGui(GameCamera& camera)
{
	// ImGui::SetNextWindowPos(ImVec2(10, 10));
	// ImGui::Begin(u8"パラメータ", nullptr);
	// //ImGui::SliderFloat(u8"的サイズ", targetScale.GetWeakPtr().lock().get(), 0, .003f, u8"%.6f");
	// //ImGui::SliderFloat(u8"Y", targetY.GetWeakPtr().lock().get(), 0, 6, u8"%.2f");
	// if (ImGui::Button(u8"リセットいがぐり"))
	// {
	// 	//auto find = gameObject.registry.FindAll(L"Bullet");
	// 	//for (auto& f : find)
	// 	//	Destroy(**f);
	// }
	// if (ImGui::Button(u8"リセットターゲット"))
	// {
	// 	//auto find = scene.FindAll(L"Target");
	// 	//for (auto& f : find)
	// 	//	Destroy(**f);
	// }
	// ImGui::End();

	// ImGui::SetNextWindowPos(ImVec2(300, 10));
	// ImGui::Begin(u8"スコア", nullptr);
	// ImGui::Text(u8"スコア: 00000032435 (未実装)");
	// ImGui::End();

	// ImGui::SetNextWindowPos(ImVec2(10, 500));
	// ImGui::Begin(u8"タイム", nullptr);
	// ImGui::Text(u8"タイム: %.2f秒", float(GameContext::Get<DX::StepTimer>().GetTotalSeconds()) - last);
	// ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(10, 300));
	ImGui::Begin(u8"目標", nullptr);
	ImGui::Text(u8"キラキラ光るゴールを目指そう");
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(10, 700));
	ImGui::Begin(u8"操作説明", nullptr);
	ImGui::Text(u8"1. 左クリック長押しで方向を決める");
	ImGui::Text(u8"2. 左クリックを離してスリングジャーンプ！");
	ImGui::End();
}

void ResultMenu::Update()
{
	if (Input::GetKeyDown(Keyboard::Keys::Escape))
		GameContext::Get<PauseHandler>().SetPaused(true);
}

void ResultMenu::RenderGui(GameCamera& camera)
{
	ImGui::SetNextWindowPosCenter();
	ImGui::SetNextWindowSize(ImVec2(230, 300));
	ImGui::Begin(u8"ゲームクリア", nullptr/*, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize*/);
	if (ImGui::Button(u8"タイトルへ", ImVec2(200, 50)))
	{
		GameContext::Get<SceneManager>().LoadScene(SceneInfo::CreateFromName("TitleScene")); // LoadSceneWithTransition(L"PlayScene", SceneTransitions::CreateRuleTransition(3));
	}
	if (ImGui::Button(u8"終了", ImVec2(200, 50)))
	{
		ApplicationHandler::ExitGame();
	}
	ImGui::End();
}

void PauseMenu::Update()
{
	if (Input::GetKeyDown(Keyboard::Keys::Escape))
		GameContext::Get<PauseHandler>().SetPaused(false);
}

void PauseMenu::RenderGui(GameCamera& camera)
{
	ImGui::SetNextWindowPosCenter();
	ImGui::SetNextWindowSize(ImVec2(230, 300));
	ImGui::Begin(u8"ポーズ", nullptr/*, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize*/);
	if (ImGui::Button(u8"続ける", ImVec2(200, 50)))
	{
		GameContext::Get<PauseHandler>().SetPaused(false);
	}
	if (ImGui::Button(u8"タイトルへ", ImVec2(200, 50)))
	{
		GameContext::Get<SceneManager>().LoadScene(SceneInfo::CreateFromName("TitleScene")); // LoadSceneWithTransition(L"PlayScene", SceneTransitions::CreateRuleTransition(3));
	}
	if (ImGui::Button(u8"終了", ImVec2(200, 50)))
	{
		ApplicationHandler::ExitGame();
	}
	ImGui::End();
}
