// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-20 22:50:01 +0900
// Modified: 2019-12-13 16:28:17 +0900

#include "pch.h"
#include "ImGuiManager.h"
#include "ImGuiStyles.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/SaveHandler.h>
#include <Framework/Context/WindowHandler.h>
#include <Utilities/Input.h>

ImGuiManager::ImGuiManager()
{
}

ImGuiManager::~ImGuiManager()
{
}

void ImGuiManager::RenderStart()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	auto wpath = GameContext::Get<SaveHandler>().GetDir() + L"GuiSettings.ini";
	m_settingFile = std::string(wpath.begin(), wpath.end());
	io.IniFilename = m_settingFile.c_str();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	{
		auto& style = ImGui::GetStyle();
		//ImGuiStyles::darkTheme(style);
		//ImGuiStyles::cyberTheme(style);
		ImGuiStyles::darkrichTheme(style);
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(GameContext::Get<WindowHandler>().GetHandle());
	ImGui_ImplDX11_Init(GameContext::Get<DX::DeviceResources>().GetD3DDevice(), GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext());

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	ImFont* font = io.Fonts->AddFontFromFileTTF("Resources/Fonts/logofont.ttf", 12.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);
	io.Fonts->AddFontDefault();
}

void ImGuiManager::Begin()
{
	// Start the Dear ImGui frame
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		if (Input::GetMouseMode() != DirectX::Mouse::Mode::MODE_ABSOLUTE)
			io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

		ImGui::NewFrame();
	}
}

void ImGuiManager::End()
{
	// Rendering
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

void ImGuiManager::RenderFinalize()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


