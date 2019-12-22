#include "pch.h"
#include "RainbowEffect.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>
#include <Utilities/MathUtils.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// 描画初期化
void RainbowEffect::RenderStart()
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// 頂点カラー(有効)
	m_basicEffect->SetVertexColorEnabled(true);
	// プリミティブオブジェクト生成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	DX::ThrowIfFailed(device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.ReleaseAndGetAddressOf()));
}

// 描画
void RainbowEffect::Render(GameCamera& camera)
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();
	auto& states = GameContext::Get<DirectX::CommonStates>();
	auto& timer = GameContext::Get<DX::StepTimer>();
	auto& transform = gameObject.GetComponent<Transform>();

	context->OMSetBlendState(states.Additive(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states.DepthDefault(), 0);
	context->RSSetState(states.CullNone());

	m_basicEffect->SetWorld(transform.GetMatrix());
	m_basicEffect->SetView(camera.view);
	m_basicEffect->SetProjection(camera.projection);

	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());

	m_primitiveBatch->Begin();

	{
		static std::vector<Vector3> vecs = {
			Vector3(1.0F, 0.0F, 0.0F),
			Vector3(0.0F, 1.0F, 0.0F),
			Vector3(0.0F, 0.0F, 1.0F),
			Vector3(1.0F, 0.0F, 0.0F),
			Vector3(0.0F, 1.0F, 0.0F),
			Vector3(0.0F, 0.0F, 1.0F),
		};

		auto renderTimer = float(timer.GetTotalSeconds()) * 20;

		std::mt19937 rand(seed);
		std::uniform_real_distribution<float> dist(0, 1);

		auto matrix = Matrix::Identity;

		for (int i = 0; i < 32; ++i)
		{
			for (auto& vec : vecs)
			{
				matrix *= Matrix::CreateFromAxisAngle(vec, 6.2831855F * dist(rand) + renderTimer / 360.0F);
			}

			float r = (1.0F + dist(rand) * 2.5F) * 5.0F;

			matrix *= Matrix::CreateFromAxisAngle(Vector3(0.0F, 1.0F, 0.0F), renderTimer / 180.0F);

			auto a = Vector4::Zero;
			auto b = Vector4::Transform(Vector4(0.0F, 0.126F * r, 0.5F * r, 1.0F), matrix);
			auto c = Vector4::Transform(Vector4(0.0F, -0.126F * r, 0.5F * r, 1.0F), matrix);
			auto d = Vector4::Transform(Vector4(0.0F, 0.0F, 0.6F * r, 1.0F), matrix);

			float col_r, col_g, col_b;
			ImGui::ColorConvertHSVtoRGB(i / 16.0F, 1.0F, 1.0F, col_r, col_g, col_b);

			m_primitiveBatch->DrawQuad(
				VertexPositionColor(a, Color(col_r, col_g, col_b, 0.9F)),
				VertexPositionColor(b, Color(col_r, col_g, col_b, 0.01F)),
				VertexPositionColor(d, Color(col_r, col_g, col_b, 0.01F)),
				VertexPositionColor(c, Color(col_r, col_g, col_b, 0.01F))
			);
		}
	}

	m_primitiveBatch->End();
}

void RainbowEffect::EditorGui()
{
	ImGui::DragInt("Seed", &seed);
	ImGui::Checkbox("Is Colorful", &isColorful);
	ImGui::ColorEdit4("Color", color.m128_f32);
}

