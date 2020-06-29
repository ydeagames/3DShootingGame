// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-16 22:40:30 +0900
// Modified: 2019-12-16 22:40:30 +0900

#include "pch.h"
#include "FontRenderer.h"
#include <Framework/ECS/GameContext.h>
#include <Utilities/StringCast.h>
#include <Framework/Components/Transform.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void FontRenderer::RenderStart()
{
	m_spriteBatch = std::make_unique<SpriteBatch>(GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext());
	auto wname = string_cast<std::wstring>(fontName);
	try
	{
		m_spriteFont = std::make_unique<SpriteFont>(GameContext::Get<DX::DeviceResources>().GetD3DDevice(), wname.c_str());
	}
	catch (std::exception)
	{
		m_spriteFont = nullptr;
	}
}

void FontRenderer::RenderGui(GameCamera& camera)
{
	if (m_spriteFont)
	{
		m_spriteBatch->Begin(SpriteSortMode_Deferred, GameContext::Get<CommonStates>().AlphaBlend());
		auto wtext = string_cast<std::wstring>(text);
		m_spriteFont->DrawString(m_spriteBatch.get(), wtext.c_str(), gameObject.GetComponent<Transform>().position, color);
		m_spriteBatch->End();
	}
}

void FontRenderer::EditorGui()
{
	{
		std::string tmpname = fontName;
		tmpname.resize(128);
		if (ImGui::InputText("Font", &tmpname[0], tmpname.size()))
		{
			fontName = std::string(tmpname.c_str());
			RenderStart();
		}
	}

	{
		std::string tmpname = text;
		tmpname.resize(512);
		if (ImGui::InputTextMultiline("Text", &tmpname[0], tmpname.size()))
		{
			text = std::string(tmpname.c_str());
		}
	}

	ImGui::ColorEdit4("Color", &color.x);
}


