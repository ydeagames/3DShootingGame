#include "pch.h"
#include "PlayScenePause.h"
#include "SceneCommons.h"
#include <Utilities/Input.h>
#include <Framework/ImGui/ImGuiManager.h>
#include <Framework/PauseHandler.h>
#include <Framework/SceneManagement/Scene.h>
#include <Framework/SceneManagement/SceneManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PlayScenePause::Build()
{
	auto& scene = context.GetScene();
	scene.renderBehind = true;

	struct Background : public Component
	{
		std::unique_ptr<GeometricPrimitive> m_plane;

		void Initialize()
		{
			std::vector<GeometricPrimitive::VertexType> vertices = {
				{ Vector3(-0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 0.0f) },
				{ Vector3(+0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 0.0f) },
				{ Vector3(+0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 1.0f) },
				{ Vector3(-0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 1.0f) },
			};
			std::vector<uint16_t> indices = {
				0, 1, 2, 0, 2, 3,
			};
			m_plane = GeometricPrimitive::CreateCustom(context.GetDR().GetD3DDeviceContext(), vertices, indices);
		}

		void Render()
		{
			m_plane->Draw(Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity, Colors::Black * .4f);
		}
	};
	auto background = scene.AddGameObject();
	background->AddComponent<Background>();

	struct Menu : public Component
	{
		void Update()
		{
			if (Input::GetKeyDown(Keyboard::Keys::Escape))
				context.GetPauseHandler().SetPaused(false);
		}

		void Render()
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::SetNextWindowSize(ImVec2(230, 230));
			ImGui::Begin(u8"ゲームメニュー", nullptr/*, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize*/);
			if (ImGui::Button(u8"ゲーム続行", ImVec2(200, 50)))
			{
				context.GetPauseHandler().SetPaused(false);
			}
			if (ImGui::Button(u8"リスタート", ImVec2(200, 50)))
			{
				context.GetSceneManager().LoadSceneWithTransition(L"PlayScene");
			}
			if (ImGui::Button(u8"タイトルに戻る", ImVec2(200, 50)))
			{
				context.GetSceneManager().LoadSceneWithTransition(L"TitleScene");
			}
			ImGui::End();
		}
	};
	auto menu = scene.AddGameObject();
	menu->AddComponent<Menu>();
}
