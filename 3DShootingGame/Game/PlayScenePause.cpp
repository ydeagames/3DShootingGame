#include "pch.h"
#include "PlayScenePause.h"
#include "SceneCommons.h"
#include <Utilities/Input.h>
#include <Framework/ImGuiManager.h>
#include <Framework/PauseHandler.h>
#include <Framework/Scene.h>
#include <Framework/SceneManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PlayScenePause::Build(GameContext& context)
{
	context.GetScene().renderBehind = true;

	struct Background : public Component
	{
		std::unique_ptr<GeometricPrimitive> m_plane;

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
		}

		void Render(GameContext& context)
		{
			m_plane->Draw(Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity, Colors::Black * .4f);
		}
	};
	auto background = GameObject::Create();
	background->AddComponent<Background>();
	context << background;

	struct PauseMenu : public Component
	{
		std::shared_ptr<ISceneBuilder> m_window;

		void Initialize(GameContext& context)
		{
			struct PauseWindow : public ISceneBuilder
			{
				std::wstring GetName() const override { return L"PauseWindow"; }
				void Build(GameContext& context)
				{
					ImGui::SetNextWindowPosCenter();
					ImGui::SetNextWindowSize(ImVec2(230, 230));
					ImGui::Begin(u8"�Q�[�����j���[", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
					if (ImGui::Button(u8"�Q�[�����s", ImVec2(200, 50)))
					{
						context.GetPauseHandler().SetPaused(context, false);
					}
					if (ImGui::Button(u8"���X�^�[�g", ImVec2(200, 50)))
					{
						context.GetSceneManager().LoadScene(L"PlayScene");
					}
					if (ImGui::Button(u8"�^�C�g���ɖ߂�", ImVec2(200, 50)))
					{
					}
					ImGui::End();
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
	auto pausedirector = GameObject::Create();
	pausedirector->AddComponent<PauseMenu>();
	context << pausedirector;
}