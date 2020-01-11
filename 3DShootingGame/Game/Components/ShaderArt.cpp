#include "pch.h"
#include "ShaderArt.h"
#include "Utilities/BinaryFile.h"
#include "Framework/ECS/GameContext.h"
#include "Framework/Components/Transform.h"
#include "Framework/Context/GameCamera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

struct ShaderArt::ConstBuffer
{
	Vector4 Time;
};

void ShaderArt::RenderStart()
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto ctx = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// プリミティブオブジェクト生成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(ctx);

	// ベーシックエフェクト
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetTextureEnabled(true);

	// コンパイルされたシェーダファイルを読み込み
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ShaderArtVS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/ShaderArtPS.cso");

	// インプットレイアウト生成
	DX::ThrowIfFailed(device->CreateInputLayout(VertexPositionTexture::InputElements,
		UINT(VertexPositionTexture::InputElementCount),
		VSData.GetData(), VSData.GetSize(),
		m_inputLayout.ReleaseAndGetAddressOf()));
	// 頂点シェーダ作成
	DX::ThrowIfFailed(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_VertexShader.ReleaseAndGetAddressOf()));
	// ピクセルシェーダ作成
	DX::ThrowIfFailed(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_PixelShader.ReleaseAndGetAddressOf()));

	// テクスチャのロード
	DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"Resources/Textures/Transition/RuleTransitionBack.png", nullptr, m_texture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"Resources/Textures/Transition/RuleTransitionRule.png", nullptr, m_texture2.ReleaseAndGetAddressOf()));

	// バッファの作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	DX::ThrowIfFailed(device->CreateBuffer(&bd, nullptr, &m_CBuffer));
}

void ShaderArt::Update()
{
	time += float(GameContext::Get<DX::StepTimer>().GetElapsedSeconds());
}

void ShaderArt::Render(GameCamera& camera)
{
	auto ctx = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// オブジェ
	auto Draw = [&](const Matrix& world, const Matrix& view, const Matrix& proj)
	{
		auto& state = GameContext::Get<CommonStates>();
		ID3D11BlendState* blendstate = state.NonPremultiplied();
		// 透明判定処理
		ctx->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
		// 深度バッファに書き込み参照する
		ctx->OMSetDepthStencilState(state.DepthRead(), 0);
		// カリングは右周り（時計回り）
		ctx->RSSetState(state.CullClockwise());

		ID3D11SamplerState* sampler[1] = { state.LinearClamp() };
		ctx->PSSetSamplers(0, 1, sampler);

		m_basicEffect->SetTexture(m_texture.Get());
		m_basicEffect->SetWorld(world);
		m_basicEffect->SetView(view);
		m_basicEffect->SetProjection(proj);

		m_basicEffect->Apply(ctx);
		ctx->IASetInputLayout(m_inputLayout.Get());

		// 定数バッファ更新
		ConstBuffer cbuff;
		auto& timer = GameContext::Get<DX::StepTimer>();
		cbuff.Time = Vector4(float(timer.GetTotalSeconds()), float(timer.GetElapsedSeconds()), time, 1);

		// 定数バッファの内容更新
		ctx->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

		// 定数バッファ反映
		ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
		ctx->VSSetConstantBuffers(1, 1, cb);
		//ctx->GSSetConstantBuffers(1, 1, cb);
		ctx->PSSetConstantBuffers(1, 1, cb);

		// 描画
		ctx->VSSetShader(m_VertexShader.Get(), nullptr, 0);
		ctx->PSSetShader(m_PixelShader.Get(), nullptr, 0);
		ctx->PSSetShaderResources(1, 1, m_texture2.GetAddressOf());
		static std::vector<VertexPositionTexture> vertices = {
			VertexPositionTexture(Vector3(+0.5f, +0.5f, 0.0f), Vector2(1.0f, 0.0f)),
			VertexPositionTexture(Vector3(-0.5f, +0.5f, 0.0f), Vector2(0.0f, 0.0f)),
			VertexPositionTexture(Vector3(-0.5f, -0.5f, 0.0f), Vector2(0.0f, 1.0f)),
			VertexPositionTexture(Vector3(+0.5f, -0.5f, 0.0f), Vector2(1.0f, 1.0f)),
		};
		static std::vector<uint16_t> indices = {
			0, 1, 2, 0, 2, 3
		};
		m_primitiveBatch->Begin();
		m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices.data(), indices.size(), vertices.data(), vertices.size());
		m_primitiveBatch->End();

		// Shader Reset
		ctx->VSSetShader(nullptr, nullptr, 0);
		ctx->PSSetShader(nullptr, nullptr, 0);
		//ctx->GSSetShader(nullptr, nullptr, 0);
	};
	// Draw(gameObject.GetComponent<Transform>().GetMatrix(), camera.view, camera.projection);
	Draw(gameObject.GetComponent<Transform>().GetMatrix(), Matrix::Identity, Matrix::Identity);
}

void ShaderArt::EditorGui()
{
	ImGui::DragFloat("Time", &time, 0.01f);
}
