#include "pch.h"
#include "PrimitiveRenderer.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Transform.h>
#include <Framework/Context/GameCamera.h>
#include <Utilities/StringCast.h>
#include "Utilities/BinaryFile.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

auto PrimitiveRenderer::primitiveModels() -> std::unordered_map<std::string, PrimitiveModel>&
{
	static std::unordered_map<std::string, PrimitiveModel> data = (
		[]() {
			std::unordered_map<std::string, PrimitiveModel> models;
			models.emplace("Cone", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCone(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCone(context, 1, 1, 32, false); }));
			models.emplace("Cube", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCube(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCube(context, 1, false); }));
			models.emplace("Cylinder", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCylinder(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCylinder(context, 1, 1, 32, false); }));
			models.emplace("Dodecahedron", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateDodecahedron(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateDodecahedron(context, 1, false); }));
			models.emplace("GeoSphere", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateGeoSphere(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateGeoSphere(context, 1, 3, false); }));
			models.emplace("Icosahedron", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateIcosahedron(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateIcosahedron(context, 1, false); }));
			models.emplace("Octahedron", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateOctahedron(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateOctahedron(context, 1, false); }));
			models.emplace("Sphere", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateSphere(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateSphere(context, 1, 16, false); }));
			models.emplace("Teapot", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTeapot(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTeapot(context, 1, 8, false); }));
			models.emplace("Tetrahedron", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTetrahedron(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTetrahedron(context, 1, false); }));
			models.emplace("Torus", PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTorus(context); }, [](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTorus(context, 1, 0.333f, 32, false); }));
			return models;
		}
	)();
	return data;
}

void PrimitiveRenderer::RenderStart()
{
	// m_model = GeometricPrimitive::CreateTeapot(GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext());
	// m_model = GeometricPrimitive::CreateGeoSphere(dr.GetD3DDeviceContext(), 1.f, 3U, false);
	auto& models = primitiveModels();
	if (models.find(model) != models.end())
	{
		auto& dr = GameContext::Get<DX::DeviceResources>();
		m_model = cullfront
			? models.at(model).GetOrCreateInverted(dr.GetD3DDeviceContext())
			: models.at(model).GetOrCreate(dr.GetD3DDeviceContext());

		// ポリゴン用エフェクト作成
		m_basicEffect = std::make_unique<BasicEffect>(dr.GetD3DDevice());

		// ライト有効
		m_basicEffect->SetLightingEnabled(lighting);
		// 環境光の色を設定
		m_basicEffect->SetAmbientLightColor(SimpleMath::Vector3(0.2f, 0.2f, 0.2f));
		// 拡散反射光の素材色を設定
		m_basicEffect->SetDiffuseColor(SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

		// テクスチャ
		m_basicEffect->SetTextureEnabled(true);
		auto texture_str = textureEnabled ? string_cast<std::wstring>(texture) : L"Resources/Textures/GridBox_Default.png";
		if (FAILED(CreateWICTextureFromFile(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			texture_str.c_str(), nullptr, m_texture.ReleaseAndGetAddressOf())))
			m_texture = nullptr;

		// シェーダー取得
		m_model->CreateInputLayout(m_basicEffect.get(), m_pInputLayout.ReleaseAndGetAddressOf());

		{
			// コンパイルされたシェーダファイルを読み込み
			BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ShadowVS.cso");
			BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/ShadowPS.cso");

			// 頂点シェーダ作成
			DX::ThrowIfFailed(dr.GetD3DDevice()->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_ShadowVertexShader.ReleaseAndGetAddressOf()));
			// ピクセルシェーダ作成
			DX::ThrowIfFailed(dr.GetD3DDevice()->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_ShadowPixelShader.ReleaseAndGetAddressOf()));
		}
		{
			// コンパイルされたシェーダファイルを読み込み
			BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ShadeVS.cso");
			BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/ShadePS.cso");

			// 頂点シェーダ作成
			DX::ThrowIfFailed(dr.GetD3DDevice()->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_ShadeVertexShader.ReleaseAndGetAddressOf()));
			// ピクセルシェーダ作成
			DX::ThrowIfFailed(dr.GetD3DDevice()->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_ShadePixelShader.ReleaseAndGetAddressOf()));
		}

		// バッファの作成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		DX::ThrowIfFailed(dr.GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_CBuffer));

		{
			// Point light at (20, 15, 20), pointed at the origin. POV up-vector is along the y-axis.
			static const Vector3 eye = Vector3(0, 20, 1);// Vector3(20.0f, 15.0f, 20.0f);
			static const Vector3 at = Vector3(0.0f, 0.0f, 0.0f);
			static const Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

			// Setup field of view and screen aspect for a square light source.
			float fieldOfView = XM_PI / 2.0f;
			float screenAspect = 1.0f;

			light.view = Matrix::CreateLookAt(eye, at, up);
			light.projection = Matrix::CreatePerspectiveFieldOfView(fieldOfView, screenAspect, 0.001f, 100.f);
			light.position = eye;
		}
	}
}

void PrimitiveRenderer::Render(GameCamera& camera)
{
	if (m_model)
	{
		auto& dr = GameContext::Get<DX::DeviceResources>();
		auto& commonStates = GameContext::Get<CommonStates>();

		auto ctx = dr.GetD3DDeviceContext();
		// ワールド行列設定
		m_basicEffect->SetWorld(gameObject.GetComponent<Transform>().GetMatrix());
		// ビュー行列設定
		m_basicEffect->SetView(camera.view);
		// プロジェクション行列設定
		m_basicEffect->SetProjection(camera.projection);
		// テクスチャ
		m_basicEffect->SetTexture(m_texture.Get());
		// エフェクトの設定
		m_basicEffect->Apply(ctx);

		m_model->Draw(m_basicEffect.get(), m_pInputLayout.Get(),
			false, false,
			[&]()
			{
				// 深度ステンシルステートの設定
				ctx->OMSetDepthStencilState(commonStates.DepthDefault(), 0);
				// ブレンドステートの設定
				ctx->OMSetBlendState(commonStates.AlphaBlend(), nullptr, 0xffffffff);
				// ラスタライザステートを設定
				ctx->RSSetState(commonStates.CullCounterClockwise());

				// Note that starting with the second frame, the previous call will display
				// warnings in VS debug output about forcing an unbind of the pixel shader
				// resource. This warning can be safely ignored when using shadow buffers
				// as demonstrated in this sample.

				// 定数バッファ更新
				ConstBuffer cbuff;
				cbuff.WorldViewProj2 = (camera.view * camera.projection).Transpose();
				cbuff.LightViewProj = (light.view * light.projection).Transpose();
				cbuff.LightPosition = light.position;

				// 定数バッファの内容更新
				ctx->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

				// 定数バッファ反映
				ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
				ctx->VSSetConstantBuffers(1, 1, cb);
				ctx->PSSetConstantBuffers(1, 1, cb);

				// シャドーマップ
				auto shadowTexture = dr.GetShadowMapShaderResourceView();
				ctx->PSSetShaderResources(1, 1, &shadowTexture);

				// In some configurations, it's possible to avoid setting a pixel shader
				// (or set PS to nullptr). Not all drivers are tolerant of this, so to be
				// safe set a minimal shader here.
				//
				// Direct3D will discard output from this shader because the render target
				// view is unbound.
				if (lighting)
				{
					ctx->VSSetShader(m_ShadeVertexShader.Get(), nullptr, 0);
					ctx->PSSetShader(m_ShadePixelShader.Get(), nullptr, 0);
				}

				// シャドーマップ
				ID3D11ShaderResourceView* null[] = { nullptr };
				ctx->PSSetShaderResources(1, 1, null);
			});
	}
}

void PrimitiveRenderer::RenderShadowMap(GameCamera& camera)
{
	if (m_model)
	{
		auto& dr = GameContext::Get<DX::DeviceResources>();
		auto& commonStates = GameContext::Get<CommonStates>();

		auto ctx = dr.GetD3DDeviceContext();
		// ワールド行列設定
		m_basicEffect->SetWorld(gameObject.GetComponent<Transform>().GetMatrix());
		// ビュー行列設定
		m_basicEffect->SetView(light.view);
		// プロジェクション行列設定
		m_basicEffect->SetProjection(light.projection);
		// エフェクトの設定
		m_basicEffect->Apply(ctx);

		m_model->Draw(m_basicEffect.get(), m_pInputLayout.Get(),
			false, false,
			[&]()
			{
				// 深度ステンシルステートの設定
				ctx->OMSetDepthStencilState(commonStates.DepthDefault(), 0);
				// ブレンドステートの設定
				ctx->OMSetBlendState(commonStates.AlphaBlend(), nullptr, 0xffffffff);
				// ラスタライザステートを設定
				ctx->RSSetState(commonStates.CullCounterClockwise());

				// Note that starting with the second frame, the previous call will display
				// warnings in VS debug output about forcing an unbind of the pixel shader
				// resource. This warning can be safely ignored when using shadow buffers
				// as demonstrated in this sample.

				// 定数バッファ更新
				ConstBuffer cbuff;
				cbuff.WorldViewProj2 = (camera.view * camera.projection).Transpose();
				cbuff.LightViewProj = (light.view * light.projection).Transpose();
				cbuff.LightPosition = light.position;

				// 定数バッファの内容更新
				ctx->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

				// 定数バッファ反映
				ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
				ctx->VSSetConstantBuffers(1, 1, cb);
				ctx->PSSetConstantBuffers(1, 1, cb);

				// In some configurations, it's possible to avoid setting a pixel shader
				// (or set PS to nullptr). Not all drivers are tolerant of this, so to be
				// safe set a minimal shader here.
				//
				// Direct3D will discard output from this shader because the render target
				// view is unbound.
				ctx->VSSetShader(m_ShadowVertexShader.Get(), nullptr, 0);
				ctx->PSSetShader(m_ShadowPixelShader.Get(), nullptr, 0);
			});
	}
}

void PrimitiveRenderer::EditorGui()
{
	{
		auto& models = primitiveModels();
		if (ImGui::BeginCombo("Model", model.c_str()))
		{
			for (auto& entry : models)
			{
				bool is_selected = (model == entry.first);
				if (ImGui::Selectable(entry.first.c_str(), is_selected))
				{
					model = entry.first;
					RenderStart();
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	{
		std::string tmpname = texture;
		tmpname.resize(128);
		if (ImGui::InputText("Texture", &tmpname[0], tmpname.size()))
		{
			texture = std::string(tmpname.c_str());
			RenderStart();
		}
	}
	{
		if (ImGui::Checkbox("Lighting", &lighting))
			RenderStart();
	}
	{
		if (ImGui::Checkbox("Cull Front", &cullfront))
			RenderStart();
	}
	{
		if (ImGui::Checkbox("Texture Enabled", &textureEnabled))
			RenderStart();
	}
}
