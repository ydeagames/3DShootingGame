#pragma once

#include "CommonStates.h"
#include "PrimitiveBatch.h"
#include "Effects.h"
#include "VertexTypes.h"
#include "SimpleMath.h"

class GameContext;
class GameCamera;

// デバッググリッド床クラス
class InfinityGridFloor
{
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// 1マスの一辺のサイズ
	float m_cellsize;

	// サイズ
	DirectX::SimpleMath::Vector2 m_size;

public:
	// コンストラクタ
	InfinityGridFloor(float cellsize, const DirectX::SimpleMath::Vector2& size);

	// デストラクタ
	~InfinityGridFloor();

	// 描画
	void draw(GameCamera& camera, DirectX::GXMVECTOR color = DirectX::Colors::Gray);
};



