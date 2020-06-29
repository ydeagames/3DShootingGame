// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-15 06:40:16 +0900

#pragma once

class GameCamera;

class MyGame
{
private:
	class ImGuiPtr;
	std::unique_ptr<ImGuiPtr> m_imgui;

public:
	MyGame();
	~MyGame();
	void Update();
	void RenderStart();
	void Render(GameCamera& camera);

	int Bench();
};


