// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-09 14:22:32 +0900
// Modified: 2020-01-06 14:19:53 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class Background : public Component
{
public:
	static constexpr const char* Identifier = "Background";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string texture = "Resources/Textures/Title/Background.png";
	bool fullScreen = true;
	bool uiSpace = true;

private:
	std::unique_ptr<DirectX::GeometricPrimitive> m_plane;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void RenderStart();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(texture), CEREAL_OPTIONAL_NVP(fullScreen), CEREAL_OPTIONAL_NVP(uiSpace));
	}

	void EditorGui();
};



