// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-20 22:50:01 +0900
// Modified: 2019-12-13 16:28:17 +0900

#pragma once

class ISceneBuilder;
class GameContext;

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

private:
	std::string m_settingFile;

public:
	// ê∂ê¨
	void RenderStart();
	// ï`âÊ
	void Begin();
	void End();
	// îjä¸
	void RenderFinalize();
};


