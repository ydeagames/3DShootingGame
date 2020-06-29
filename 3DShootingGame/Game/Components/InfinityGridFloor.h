// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-10 16:21:00 +0900
// Modified: 2020-01-24 14:01:41 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameContext;
class GameCamera;

// デバッググリッド床クラス
class InfinityGridFloor : public Component
{
public:
	static constexpr const char* Identifier = "InfinityGridFloor";

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
	// 1マスの一辺のサイズ
	float cellsize = 2.f;

	// サイズ
	DirectX::SimpleMath::Vector2 size = DirectX::SimpleMath::Vector2(200, 200);

	// 色
	DirectX::SimpleMath::Color color = DirectX::Colors::Gray;

public:
	// 描画初期化
	void RenderStart();
	// 描画
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(cellsize), CEREAL_OPTIONAL_NVP(size), CEREAL_OPTIONAL_NVP(color));
	}

	void EditorGui();
};


