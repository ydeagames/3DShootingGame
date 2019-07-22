#include "pch.h"
#include "TitleScene.h"
#include "SceneCommons.h"
#include <Utilities/Input.h>
#include <Framework/ImGuiManager.h>
#include <Framework/PauseHandler.h>
#include <Framework/Scene.h>
#include <Framework/SceneManager.h>
#include <Framework/ExitHandler.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void TitleScene::Build(GameContext& context)
{
	auto& scene = context.GetScene();

	struct Background : public Component
	{
		std::unique_ptr<GeometricPrimitive> m_plane;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

		void Initialize(GameContext& context)
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

			DX::ThrowIfFailed(CreateWICTextureFromFile(
				context.GetDR().GetD3DDevice(), context.GetDR().GetD3DDeviceContext(),
				L"Resources/Textures/Title/Background.png", nullptr, m_texture.ReleaseAndGetAddressOf()));
		}

		void Render(GameContext& context)
		{
			m_plane->Draw(Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity, Colors::White, m_texture.Get());
		}
	};
	auto background = scene.AddGameObject();
	background->AddComponent<Background>();

	struct Menu : public Component
	{
		std::shared_ptr<ISceneBuilder> m_window;

		void Initialize(GameContext& context)
		{
			struct PauseWindow : public ISceneBuilder
			{
				std::wstring GetName() const override { return L"PauseWindow"; }
				bool showCredit = false;
				void Build(GameContext& context)
				{
					ImGui::SetNextWindowPosCenter();
					ImGui::SetNextWindowSize(ImVec2(230, 300));
					ImGui::Begin(u8"スリングヒーローズ", nullptr/*, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize*/);
					if (ImGui::Button(u8"ゲームスタート", ImVec2(200, 50)))
					{
						context.GetSceneManager().LoadSceneWithTransition(L"PlayScene");
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
						ExitGame();
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
			};

			m_window = std::make_shared<PauseWindow>();
			context.GetGuiManager().Add(m_window);
		}

		void Update(GameContext& context)
		{
			if (Input::GetKeyDown(Keyboard::Keys::Escape))
				context.GetPauseHandler().SetPaused(context, false);
		}

		void Finalize(GameContext& context)
		{
			Destroy(*m_window);
		}
	};
	auto menu = scene.AddGameObject();
	menu->AddComponent<Menu>();
}
