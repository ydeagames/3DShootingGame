//======================================================
// File Name	: GridFloor.h
// Summary		: デバッググリッド床
// Date			: 2019/4/25
// Author		: Takafumi Ban
//======================================================
#pragma once

#include "CommonStates.h"
#include "PrimitiveBatch.h"
#include "Effects.h"
#include "VertexTypes.h"
#include "SimpleMath.h"

// デバッググリッド床クラス
class GridFloor
{
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// コモンステートへのポインタ
	DirectX::CommonStates* m_states;

	// 床の一辺のサイズ
	float m_size;

	// 分割数
	int m_divs;

public:
	// コンストラクタ
	GridFloor(ID3D11Device* device, ID3D11DeviceContext* context, DirectX::CommonStates* states, float size, int divs);

	// デストラクタ
	~GridFloor();

	// 描画
	void draw(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::GXMVECTOR color = DirectX::Colors::Gray);
};



