﻿#pragma once
#include "Framework/ECS/Scene.h"
#include "Framework/ECS/Component.h"

class GameCamera;

// フェード実装
class ShaderArt : public Component
{
public:
	float time = 0;

private:
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>	m_primitiveBatch;
	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;
	// テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;

	std::unique_ptr<DirectX::GeometricPrimitive> m_plane;
	bool m_changed = false;
	struct ConstBuffer;

public:
	void RenderStart();
	void Update();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(time));
	}

	void EditorGui();
};