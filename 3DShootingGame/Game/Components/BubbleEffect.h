// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2020-01-15 16:08:06 +0900
// Modified: 2020-01-15 16:08:06 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class BubbleEffect : public Component
{
public:
	static constexpr const char* Identifier = "BubbleEffect";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string texture;
	float time = 0;
	int count;
	int seed;

private:
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void Update();
	void RenderStart();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(texture), CEREAL_OPTIONAL_NVP(time), CEREAL_OPTIONAL_NVP(count), CEREAL_OPTIONAL_NVP(seed));
	}

	void EditorGui();
};


