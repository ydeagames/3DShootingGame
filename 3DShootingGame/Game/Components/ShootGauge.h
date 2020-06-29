// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-01-15 16:08:06 +0900
// Modified: 2020-01-27 14:06:31 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class ShootGauge : public Component
{
public:
	static constexpr const char* Identifier = "ShootGauge";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string texture;
	float time = 0;
	float percent = 0;
	float speed = 0;
	entt::entity player = entt::null;

private:
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void Update();
	void RenderStart();
	void RenderGui(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(texture), CEREAL_OPTIONAL_NVP(player));
	}

	void EditorGui();
};


