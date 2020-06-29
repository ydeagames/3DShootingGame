// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-12 20:06:33 +0900
// Modified: 2019-12-16 06:37:33 +0900

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
		if (ImGui::InputText("Model", &tmpname[0], tmpname.size()))
		{
			model = std::string(tmpname.c_str());
			RenderStart();
		}
	}
	{
		if (ImGui::Checkbox("Cull Front", &cullfront))
			RenderStart();
	}
	{
		if (ImGui::Checkbox("Wireframe", &wireframe))
			RenderStart();
	}
}


