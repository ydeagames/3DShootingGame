// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-09 14:22:32 +0900
// Modified: 2019-12-12 20:06:33 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class ModelRenderer : public Component
{
public:
	static constexpr const char* Identifier = "ModelRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string model = "Resources/Models/igaguri.cmo";
	bool cullfront = false;
	bool wireframe = false;

private:
	// ƒ‚ƒfƒ‹
	std::unique_ptr<DirectX::Model> m_model = nullptr;

public:
	void RenderStart();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(model), CEREAL_OPTIONAL_NVP(cullfront), CEREAL_OPTIONAL_NVP(wireframe));
	}

	void EditorGui();
};


