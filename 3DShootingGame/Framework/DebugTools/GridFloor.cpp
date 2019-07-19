//======================================================
// File Name	: GridFloor.cpp
// Summary		: デバッググリッド床
// Date			: 2019/4/25
// Author		: Takafumi Ban
//======================================================
#include "pch.h"

#include "GridFloor.h"

#include <algorithm>

// コンストラクタ
GridFloor::GridFloor(ID3D11Device* device, ID3D11DeviceContext* context, DirectX::CommonStates* states, float size, int divs) : m_size(size), m_divs(divs), m_states(states)
{
	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// 頂点カラー(有効)
	m_basicEffect->SetVertexColorEnabled(true);
	// プリミティブオブジェクト生成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.GetAddressOf());
}

// デストラクタ
GridFloor::~GridFloor()
{
	// エフェクトを解放する
	m_basicEffect.reset();
	// 入力レイアウトを解放する
	m_pInputLayout.Reset();
}

// 描画
void GridFloor::draw(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, DirectX::GXMVECTOR color)
{
	DirectX::SimpleMath::Matrix world;

	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);


	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());

	m_primitiveBatch->Begin();

	const DirectX::XMVECTORF32 xAxis = { m_size, 0.f, 0.f };
	const DirectX::XMVECTORF32 yAxis = { 0.f, 0.f, m_size };

	size_t divs = std::max<size_t>(1, m_divs);
	DirectX::FXMVECTOR origin = DirectX::g_XMZero;
	for (size_t i = 0; i <= divs; ++i)
	{
		float fPercent = float(i) / float(divs);
		fPercent = (fPercent * 1.0f) - 0.5f;
		DirectX::XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
		vScale = DirectX::XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(vScale, yAxis * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(vScale, yAxis * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= divs; i++)
	{
		FLOAT fPercent = float(i) / float(divs);
		fPercent = (fPercent * 1.0f) - 0.5f;
		DirectX::XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
		vScale = DirectX::XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(vScale, xAxis * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(vScale, xAxis * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	m_primitiveBatch->End();
}

