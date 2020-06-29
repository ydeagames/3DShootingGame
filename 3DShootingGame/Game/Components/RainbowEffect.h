// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-10 16:21:00 +0900
// Modified: 2019-12-23 10:56:15 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameContext;
class GameCamera;

class RainbowEffect : public Component
{
public:
	static constexpr const char* Identifier = "RainbowEffect";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

private:
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

public:
	// シード
	int seed = 0;

	// カラフル
	bool isColorful = true;

	// 色
	DirectX::SimpleMath::Color color = DirectX::SimpleMath::Color(DirectX::Colors::Gray);

public:
	// 描画初期化
	void RenderStart();
	// 描画
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(seed), CEREAL_OPTIONAL_NVP(isColorful), CEREAL_OPTIONAL_NVP(color));
	}

	void EditorGui();
};


