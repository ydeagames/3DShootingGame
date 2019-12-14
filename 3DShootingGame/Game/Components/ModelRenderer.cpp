#include "pch.h"
#include "ModelRenderer.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Transform.h>
#include <Framework/Context/GameCamera.h>
#include <Utilities/StringCast.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void ModelRenderer::RenderStart()
{
	auto& dr = GameContext::Get<DX::DeviceResources>();
	auto model_str = string_cast<std::wstring>(model);
	try
	{
		m_model = Model::CreateFromCMO(dr.GetD3DDevice(), model_str.c_str(), GameContext::Get<EffectFactory>(), !cullfront);
	}
	catch (std::exception)
	{
		m_model = nullptr;
	}
}

void ModelRenderer::Render(GameCamera& camera)
{
	if (m_model)
	{
		auto& dr = GameContext::Get<DX::DeviceResources>();

		m_model->Draw(
			dr.GetD3DDeviceContext(),
			GameContext::Get<CommonStates>(),
			gameObject.GetComponent<Transform>().GetMatrix(),
			camera.view,
			camera.projection,
			wireframe
		);
	}
}

void ModelRenderer::EditorGui()
{
	{
		std::string tmpname = model;
		tmpname.resize(128);
		ImGui::InputText("Model##ModelRenderer", &tmpname[0], tmpname.size());
		auto tmpname2 = std::string(tmpname.c_str());
		if (model != tmpname2)
		{
			model = tmpname2;
			RenderStart();
		}
	}
	{
		bool cf = cullfront;
		ImGui::Checkbox("Cull Front##ModelRenderer", &cullfront);
		if (cf != cullfront)
			RenderStart();
	}
	{
		bool cf = wireframe;
		ImGui::Checkbox("Wireframe##ModelRenderer", &wireframe);
		if (cf != wireframe)
			RenderStart();
	}
}
