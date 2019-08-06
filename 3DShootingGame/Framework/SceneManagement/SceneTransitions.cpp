#include "pch.h"
#include "SceneTransitions.h"
#include <Framework/GameContext.h>
#include <Utilities/BinaryFile.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace SceneTransitions
{
	// フェード実装
	struct FadeTransition : public SceneTransition
	{
		float m_time;
		float m_totalTime;

		FadeTransition(float totalTime)
			: m_time(0)
			, m_totalTime(totalTime) {}

		std::unique_ptr<GeometricPrimitive> m_plane;
		bool changed = false;

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

		void Update(GameContext& context)
		{
			m_time += float(context.GetTimer().GetElapsedSeconds());
			if (!changed && m_time >= m_totalTime / 2)
			{
				unloadBefore(context);
				loadAfter(context);
				changed = true;
			}
			if (m_time > m_totalTime)
				Destroy(*this);
		}

		void Render(GameContext& context)
		{
			float alpha = std::abs((m_time / m_totalTime - .5f) * 2);
			alpha = 1 - alpha * alpha;
			m_plane->Draw(Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity, Colors::Black * alpha);
		}

		void Finalize(GameContext& context)
		{

		}
	};

	std::shared_ptr<SceneTransition> CreateFadeTransition(float duration)
	{
		return std::make_shared<FadeTransition>(duration);
	}

	// フェード実装
	struct RuleTransition : public SceneTransition
	{
#pragma warning(push)
#pragma warning(disable:4324)
		struct alignas(16) ConstBuffer
		{
			DirectX::SimpleMath::Matrix		matWorld;
			DirectX::SimpleMath::Matrix		matView;
			DirectX::SimpleMath::Matrix		matProj;
			DirectX::SimpleMath::Vector4	Diffuse;
			float time;
			float range;
		};
#pragma warning(pop)

		float m_time;
		float m_totalTime;

		// プリミティブバッチ
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>	m_primitiveBatch;
		// インプットレイアウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		// 頂点シェーダ
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
		// ピクセルシェーダ
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
		// バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;
		// テクスチャハンドル
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;

		RuleTransition(float totalTime)
			: m_time(0)
			, m_totalTime(totalTime) {}

		std::unique_ptr<GeometricPrimitive> m_plane;
		bool changed = false;

		void Initialize(GameContext& context)
		{
			auto device = context.GetDR().GetD3DDevice();
			auto ctx = context.GetDR().GetD3DDeviceContext();

			// プリミティブオブジェクト生成
			m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(ctx);

			// コンパイルされたシェーダファイルを読み込み
			BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/RuleTransitionVS.cso");
			BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/RuleTransitionPS.cso");

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

		void Update(GameContext& context)
		{
			m_time += float(context.GetTimer().GetElapsedSeconds());
			if (!changed && m_time >= m_totalTime / 2)
			{
				unloadBefore(context);
				loadAfter(context);
				changed = true;
			}
			if (m_time > m_totalTime)
				Destroy(*this);
		}

		void Render(GameContext& context)
		{
			float range = (m_time / m_totalTime) * 2;

			auto ctx = context.GetDR().GetD3DDeviceContext();

			// オブジェ
			auto Draw = [&](const Matrix& world, const Matrix& view, const Matrix& proj)
			{
				ID3D11BlendState* blendstate = context.GetStates().NonPremultiplied();
				// 透明判定処理
				ctx->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
				// 深度バッファに書き込み参照する
				ctx->OMSetDepthStencilState(context.GetStates().DepthRead(), 0);
				// カリングは右周り（時計回り）
				ctx->RSSetState(context.GetStates().CullClockwise());

				ID3D11SamplerState* sampler[1] = { context.GetStates().LinearClamp() };
				ctx->PSSetSamplers(0, 1, sampler);

				ctx->IASetInputLayout(m_inputLayout.Get());

				// 定数バッファ更新
				ConstBuffer cbuff;
				cbuff.matView = view.Transpose();
				cbuff.matProj = proj.Transpose();
				cbuff.matWorld = world.Transpose();
				cbuff.Diffuse = Vector4(1, 1, 1, 1);
				cbuff.time = float(context.GetTimer().GetTotalSeconds());
				cbuff.range = std::min(2.f, std::max(0.f, range));

				// 定数バッファの内容更新
				ctx->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

				// 定数バッファ反映
				ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
				ctx->VSSetConstantBuffers(0, 1, cb);
				//ctx->GSSetConstantBuffers(0, 1, cb);
				ctx->PSSetConstantBuffers(0, 1, cb);

				// 描画
				ctx->VSSetShader(m_VertexShader.Get(), nullptr, 0);
				ctx->PSSetShader(m_PixelShader.Get(), nullptr, 0);
				ctx->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
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
			Draw(Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity);
		}

		void Finalize(GameContext& context)
		{

		}
	};

	std::shared_ptr<SceneTransition> CreateRuleTransition(float duration)
	{
		return std::make_shared<RuleTransition>(duration);
	}
}
