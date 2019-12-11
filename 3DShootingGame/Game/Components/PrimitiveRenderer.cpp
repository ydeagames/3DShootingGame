#include "pch.h"
#include "PrimitiveRenderer.h"
#include <Common/DeviceResources.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Transform.h>
#include <Framework/Context/GameCamera.h>
#include <Utilities/StringCast.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

auto PrimitiveRenderer::primitiveModels() -> std::unordered_map<std::string, PrimitiveModel> &
{
	static std::unordered_map<std::string, PrimitiveModel> data = (
		[]() {
			std::unordered_map<std::string, PrimitiveModel> models;
			models.emplace("Cone",			PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCone(context); },			[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCone(context, 1, 1, 32, false); }			));
			models.emplace("Cube",			PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCube(context); },			[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCube(context, 1, false); }					));
			models.emplace("Cylinder",		PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCylinder(context); },		[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateCylinder(context, 1, 1, 32, false); }		));
			models.emplace("Dodecahedron",	PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateDodecahedron(context); },	[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateDodecahedron(context, 1, false); }			));
			models.emplace("GeoSphere",		PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateGeoSphere(context); },		[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateGeoSphere(context, 1, 3, false); }			));
			models.emplace("Icosahedron",	PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateIcosahedron(context); },	[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateIcosahedron(context, 1, false); }			));
			models.emplace("Octahedron",	PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateOctahedron(context); },		[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateOctahedron(context, 1, false); }			));
			models.emplace("Sphere",		PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateSphere(context); },			[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateSphere(context, 1, 16, false); }			));
			models.emplace("Teapot",		PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTeapot(context); },			[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTeapot(context, 1, 8, false); }			));
			models.emplace("Tetrahedron",	PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTetrahedron(context); },	[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTetrahedron(context, 1, false); }			));
			models.emplace("Torus",			PrimitiveModel([](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTorus(context); },			[](ID3D11DeviceContext* context) { return DirectX::GeometricPrimitive::CreateTorus(context, 1, 0.333f, 32, false); }	));
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

		auto texture_str = string_cast<std::wstring>(texture);

		if (FAILED(CreateWICTextureFromFile(
			dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
			texture_str.c_str(), nullptr, m_texture.ReleaseAndGetAddressOf())))
			m_texture = nullptr;
		else
		{
			// ポリゴン用エフェクト作成
			m_basicEffect = std::make_unique<BasicEffect>(dr.GetD3DDevice());
			m_basicEffect->SetTextureEnabled(true);

			// ライト有効
			m_basicEffect->SetLightingEnabled(true);
			// 環境光の色を設定
			m_basicEffect->SetAmbientLightColor(SimpleMath::Vector3(0.2f, 0.2f, 0.2f));
			// 拡散反射光の素材色を設定
			m_basicEffect->SetDiffuseColor(SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

			// シェーダー取得
			m_model->CreateInputLayout(m_basicEffect.get(), m_pInputLayout.ReleaseAndGetAddressOf());
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
		// エフェクトの設定
		m_basicEffect->Apply(ctx);
		// 深度ステンシルステートの設定
		ctx->OMSetDepthStencilState(commonStates.DepthDefault(), 0);
		// ブレンドステートの設定
		ctx->OMSetBlendState(commonStates.AlphaBlend(), nullptr, 0xffffffff);
		// ラスタライザステートを設定
		ctx->RSSetState(commonStates.CullClockwise());
		// 入力レイアウトの設定
		ctx->IASetInputLayout(m_pInputLayout.Get());

		m_basicEffect->SetTexture(m_texture.Get());

		m_model->Draw(m_basicEffect.get(), m_pInputLayout.Get());
		//m_model->Draw(gameObject->transform->GetMatrix(), context.GetCamera().view, context.GetCamera().projection, Colors::White, m_texture.Get(), false);
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
		ImGui::InputText("Texture##Background", &tmpname[0], tmpname.size());
		auto tmpname2 = std::string(tmpname.c_str());
		if (texture != tmpname2)
		{
			texture = tmpname2;
			RenderStart();
		}
	}
	{
		bool cf = cullfront;
		ImGui::Checkbox("Cull Front##Background", &cullfront);
		if (cf != cullfront)
			RenderStart();
	}
}
