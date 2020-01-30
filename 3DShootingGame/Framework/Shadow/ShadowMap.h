#pragma once

class GameCamera;

class ShadowMap
{
private:
	DX::DeviceResources* m_deviceResources;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

public:
	ShadowMap();
	
private:
	// 3Dオブジェクト描画用
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;		// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;		// ピクセル・シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShaderNoSM = nullptr;	// ピクセル・シェーダ(シャドウ マップなし)
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState = nullptr;			// ブレンド・ステート・オブジェクト
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState = nullptr;	// ラスタライザ・ステート・オブジェクト
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState = nullptr;	// 深度/ステンシル・ステート・オブジェクト

	// シャドウ マップ描画用
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShaderShadow = nullptr;		// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStateShadow = nullptr;	// ラスタライザ・ステート・オブジェクト

	Microsoft::WRL::ComPtr<ID3D11Buffer> g_pCBuffer = nullptr;		// 定数バッファ

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_textureSampler[2] = { nullptr, nullptr };		// サンプラー

	// シャドウ・マッピング用
	bool m_shadowMappingEnabled = true;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowMap = nullptr;			// シャドウ・マップ
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_shadowMapDSView = nullptr;	// 深度/ステンシル・ビュー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowMapSRView = nullptr;	// シェーダ・リソース・ビュー
	D3D11_VIEWPORT            m_viewPortShadowMap[1] = {};       // ビューポート

	// 定数バッファのデータ定義
	struct cbCBuffer {
		DirectX::SimpleMath::Matrix SMViewProj; // ワールド×ビュー×透視変換行列(シャドウマップ用)
	};

	// 定数バッファのデータ
	struct cbCBuffer m_constBuffer;

public:
	ID3D11ShaderResourceView* GetShadowMapSRView() const { return m_shadowMapSRView.Get(); };
	void SetEnabled(bool enabled) { m_shadowMappingEnabled = enabled; };
	bool IsEnabled() const { return m_shadowMappingEnabled; };

private:
	void CreateShaderObj();
	void CreateShaderShadow();
	void InitShadowMap(int dimension);

public:
	// 生成
	void RenderStart();
	// 描画
	void SetShadowMode();
	void SetRenderMode();
	void ApplyMode(bool shadowMode);
	void ApplyShadowMode();
	void ApplyRenderMode();
};
