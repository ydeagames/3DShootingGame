#pragma once
#include "Utilities/wavefrontobj.h"

class GameCamera;

class ShadowMap
{
private:
	// 起動時の描画領域サイズ
	SIZE		g_sizeWindow = { 640, 480 };		// ウインドウのクライアント領域

	// インターフェイス
	ID3D11RenderTargetView* g_pRenderTargetView = NULL; // 描画ターゲット・ビュー
	D3D11_VIEWPORT          g_ViewPort[1];				// ビューポート

	ID3D11Texture2D* g_pDepthStencil = NULL;		// 深度/ステンシル
	ID3D11DepthStencilView* g_pDepthStencilView = NULL;	// 深度/ステンシル・ビュー

	Microsoft::WRL::ComPtr<ID3D11InputLayout> g_pInputLayout = NULL;            // 入力レイアウト・オブジェクト

	// 3Dオブジェクト描画用
	Microsoft::WRL::ComPtr<ID3D11VertexShader> g_pVertexShader = NULL;		// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> g_pPixelShader = NULL;		// ピクセル・シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> g_pPixelShaderNoSM = NULL;	// ピクセル・シェーダ(シャドウ マップなし)
	Microsoft::WRL::ComPtr<ID3D11BlendState> g_pBlendState = NULL;			// ブレンド・ステート・オブジェクト
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> g_pRasterizerState = NULL;	// ラスタライザ・ステート・オブジェクト
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> g_pDepthStencilState = NULL;	// 深度/ステンシル・ステート・オブジェクト

	// シャドウ マップ描画用
	Microsoft::WRL::ComPtr<ID3D11VertexShader> g_pVertexShaderShadow = NULL;		// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> g_pRasterizerStateShadow = NULL;	// ラスタライザ・ステート・オブジェクト

	Microsoft::WRL::ComPtr<ID3D11Buffer> g_pCBuffer = NULL;		// 定数バッファ

	Microsoft::WRL::ComPtr<ID3D11SamplerState> g_pTextureSampler[3] = { NULL, NULL, NULL };		// サンプラー

	// シャドウ・マッピング用
	bool g_bShadowMappingMode = true;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> g_pShadowMap = NULL;			// シャドウ・マップ
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> g_pShadowMapDSView = NULL;	// 深度/ステンシル・ビュー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_pShadowMapSRView = NULL;	// シェーダ・リソース・ビュー
	D3D11_VIEWPORT            g_ViewPortShadowMap[1];       // ビューポート

	// 深度バッファのモード
	bool g_bDepthMode = true;

	// スタンバイモード
	bool g_bStandbyMode = false;

	// 描画ターゲットをクリアする値(RGBA)
	float g_ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

	float g_fEye = 2.5f;                       // 視点までの距離
	DirectX::SimpleMath::Vector3 g_vLightPos = { 3.0f, 3.0f, -3.0f };   // 光源座標(ワールド座標系)
	DirectX::SimpleMath::Matrix matShadowMapView;
	DirectX::SimpleMath::Matrix matShadowMapProj;

	// 定数バッファのデータ定義
	struct cbCBuffer {
		DirectX::SimpleMath::Matrix SMWorldViewProj; // ワールド×ビュー×透視変換行列(シャドウマップ用)
	};

	// 定数バッファのデータ
	struct cbCBuffer g_cbCBuffer;

	// メッシュ情報
	CWavefrontObj       g_wfObjKuma;
	CWavefrontMtl       g_wfMtl;     // マテリアル

public:
	ID3D11ShaderResourceView* GetShadowMapSRView() const { return g_pShadowMapSRView.Get(); };

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> modelTexture;
	std::vector<DirectX::VertexPositionNormalTexture> modelVertices;
	std::vector<uint16_t> modelIndices;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>> primitiveBatch;
	std::unique_ptr<DirectX::BasicEffect> basicEffect;

private:
	void CreateShaderObj();
	void CreateShaderShadow();
	void InitBackBuffer();
	void InitShadowMap();

public:
	// 生成
	void RenderStart();
	// 描画
	void Begin();
	void DrawObj(bool drawShadowMap);
	void RenderObj(bool drawShadowMap);
	void Render(GameCamera& camera);
	void End();
};
