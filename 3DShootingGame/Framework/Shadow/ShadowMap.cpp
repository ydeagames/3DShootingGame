#include "pch.h"
#include "ShadowMap.h"
#include "Framework/ECS/GameContext.h"
#include "Utilities/BinaryFile.h"
#include "Framework/Context/GameCamera.h"
#include "Framework/Shadow/Light.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

ShadowMap::ShadowMap()
	: m_deviceResources(&GameContext::Get<DX::DeviceResources>())
	, device(m_deviceResources->GetD3DDevice())
	, context(m_deviceResources->GetD3DDeviceContext())
{
}

void ShadowMap::CreateShaderObj()
{
	// **********************************************************
	// 頂点シェーダのコードをロード
	BinaryFile pBlobVS = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_VS.cso");
	// 頂点シェーダの作成
	DX::ThrowIfFailed(device->CreateVertexShader(
		pBlobVS.GetData(), // バイト・コードへのポインタ
		pBlobVS.GetSize(),    // バイト・コードの長さ
		nullptr,
		m_vertexShader.ReleaseAndGetAddressOf())); // 頂点シェーダを受け取る変数

	// **********************************************************
	// ピクセル・シェーダのコードをコンパイル
	BinaryFile pBlobPS = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_PS.cso");
	// ピクセル・シェーダの作成
	DX::ThrowIfFailed(device->CreatePixelShader(
		pBlobPS.GetData(), // バイト・コードへのポインタ
		pBlobPS.GetSize(),    // バイト・コードの長さ
		nullptr,
		m_pixelShader.ReleaseAndGetAddressOf())); // ピクセル・シェーダを受け取る変数

	// ピクセル・シェーダのコードをコンパイル
	BinaryFile pBlobPSNOSM = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_PS_NOSM.cso");
	// ピクセル・シェーダの作成
	DX::ThrowIfFailed(device->CreatePixelShader(
		pBlobPSNOSM.GetData(), // バイト・コードへのポインタ
		pBlobPSNOSM.GetSize(),    // バイト・コードの長さ
		nullptr,
		m_pixelShaderNoSM.ReleaseAndGetAddressOf())); // ピクセル・シェーダを受け取る変数

	// **********************************************************
	// ラスタライザ・ステート・オブジェクトの作成
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // 普通に描画する
	RSDesc.CullMode = D3D11_CULL_BACK;    // 表面を描画する
	RSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.DepthClipEnable = TRUE;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	DX::ThrowIfFailed(device->CreateRasterizerState(&RSDesc, m_rasterizerState.GetAddressOf()));
}

void ShadowMap::CreateShaderShadow()
{
	// **********************************************************
	// 頂点シェーダのコードをコンパイル
	BinaryFile pBlobVS = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_VS_SM.cso");
	// 頂点シェーダの作成
	DX::ThrowIfFailed(device->CreateVertexShader(
		pBlobVS.GetData(), // バイト・コードへのポインタ
		pBlobVS.GetSize(),    // バイト・コードの長さ
		nullptr,
		m_vertexShaderShadow.ReleaseAndGetAddressOf())); // 頂点シェーダを受け取る変数

	// **********************************************************
	// ラスタライザ・ステート・オブジェクトの作成
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // 普通に描画する
	RSDesc.CullMode = D3D11_CULL_FRONT;   // 表面を描画する
	RSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.DepthClipEnable = TRUE;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	DX::ThrowIfFailed(device->CreateRasterizerState(&RSDesc, m_rasterizerStateShadow.GetAddressOf()));
}

void ShadowMap::InitShadowMap(int dimension)
{
	// シャドウ マップの作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = UINT(dimension);   // 幅
	descDepth.Height = UINT(dimension);  // 高さ
	descDepth.MipLevels = 1;       // ミップマップ レベル数
	descDepth.ArraySize = 1;       // 配列サイズ
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;  // フォーマット
	descDepth.SampleDesc.Count = 1;  // マルチサンプリングの設定
	descDepth.SampleDesc.Quality = 0;  // マルチサンプリングの品質
	descDepth.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // 深度/ステンシル、シェーダ リソース ビューとして使用
	descDepth.CPUAccessFlags = 0;   // CPUからはアクセスしない
	descDepth.MiscFlags = 0;   // その他の設定なし
	DX::ThrowIfFailed(device->CreateTexture2D(
		&descDepth,         // 作成する2Dテクスチャの設定
		nullptr,               // 
		m_shadowMap.ReleaseAndGetAddressOf()));     // 作成したテクスチャを受け取る変数

	// 深度/ステンシル ビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;            // ビューのフォーマット
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	DX::ThrowIfFailed(device->CreateDepthStencilView(
		m_shadowMap.Get(),         // 深度/ステンシル・ビューを作るテクスチャ
		&descDSV,             // 深度/ステンシル・ビューの設定
		m_shadowMapDSView.ReleaseAndGetAddressOf())); // 作成したビューを受け取る変数

	// シェーダ リソース ビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT; // フォーマット
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
	srDesc.Texture2D.MostDetailedMip = 0;   // 最初のミップマップ レベル
	srDesc.Texture2D.MipLevels = UINT(-1);  // すべてのミップマップ レベル
	DX::ThrowIfFailed(device->CreateShaderResourceView(
		m_shadowMap.Get(),          // アクセスするテクスチャ リソース
		&srDesc,               // シェーダ リソース ビューの設定
		m_shadowMapSRView.ReleaseAndGetAddressOf()));  // 受け取る変数

	// ビューポートの設定
	m_viewPortShadowMap[0].TopLeftX = 0.0f;				// ビューポート領域の左上X座標。
	m_viewPortShadowMap[0].TopLeftY = 0.0f;				// ビューポート領域の左上Y座標。
	m_viewPortShadowMap[0].Width = float(dimension);	// ビューポート領域の幅
	m_viewPortShadowMap[0].Height = float(dimension);	// ビューポート領域の高さ
	m_viewPortShadowMap[0].MinDepth = 0.0f;				// ビューポート領域の深度値の最小値
	m_viewPortShadowMap[0].MaxDepth = 1.0f;				// ビューポート領域の深度値の最大値
}

void ShadowMap::RenderStart()
{
	// **********************************************************
	// シェーダのコンパイル
	CreateShaderObj();
	CreateShaderShadow();

	// **********************************************************
	// 定数バッファの定義
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 定数バッファ
	cBufferDesc.CPUAccessFlags = 0;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	// 定数バッファの作成
	cBufferDesc.ByteWidth = sizeof(ShadowParameters); // バッファ・サイズ
	DX::ThrowIfFailed(device->CreateBuffer(&cBufferDesc, nullptr, g_pCBuffer.ReleaseAndGetAddressOf()));

	// **********************************************************
	// ブレンド・ステート・オブジェクトの作成
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	DX::ThrowIfFailed(device->CreateBlendState(&BlendState, m_blendState.ReleaseAndGetAddressOf()));

	// **********************************************************
	// 深度/ステンシル・ステート・オブジェクトの作成
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable = TRUE; // 深度テストあり
	DepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 書き込む
	DepthStencil.DepthFunc = D3D11_COMPARISON_LESS; // 手前の物体を描画
	DepthStencil.StencilEnable = FALSE; // ステンシル・テストなし
	DepthStencil.StencilReadMask = 0xff;     // ステンシル読み込みマスク。
	DepthStencil.StencilWriteMask = 0xff;     // ステンシル書き込みマスク。
	// 面が表を向いている場合のステンシル・テストの設定
	DepthStencil.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 常に成功
	// 面が裏を向いている場合のステンシル・テストの設定
	DepthStencil.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 常に成功
	DX::ThrowIfFailed(device->CreateDepthStencilState(&DepthStencil, &m_depthStencilState));

	// **********************************************************
	// サンプラーの作成
	D3D11_SAMPLER_DESC descSampler;
	descSampler.Filter = D3D11_FILTER_ANISOTROPIC;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	descSampler.MipLODBias = 0.0f;
	descSampler.MaxAnisotropy = 2;
	descSampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
	descSampler.BorderColor[0] = 0.0f;
	descSampler.BorderColor[1] = 0.0f;
	descSampler.BorderColor[2] = 0.0f;
	descSampler.BorderColor[3] = 0.0f;
	descSampler.MinLOD = -FLT_MAX;
	descSampler.MaxLOD = FLT_MAX;
	DX::ThrowIfFailed(device->CreateSamplerState(&descSampler, &m_textureSampler[0]));

	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.BorderColor[0] = 1.0f;
	descSampler.BorderColor[1] = 1.0f;
	descSampler.BorderColor[2] = 1.0f;
	descSampler.BorderColor[3] = 1.0f;
	// DX::ThrowIfFailed(device->CreateSamplerState(&descSampler, &g_pTextureSampler[1]));

	descSampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	descSampler.MaxAnisotropy = 1;
	descSampler.MipLODBias = 0;
	descSampler.MinLOD = -FLT_MAX;
	descSampler.MaxLOD = +FLT_MAX;
	DX::ThrowIfFailed(device->CreateSamplerState(&descSampler, &m_textureSampler[1]));

	// **********************************************************
	// シャドウ・マップの作成
	InitShadowMap(1024);
}

void ShadowMap::SetShadowMode()
{
	// 深度/ステンシルのクリア
	context->ClearDepthStencilView(m_shadowMapDSView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	context->ClearState();
	
	// RSにビューポートを設定
	context->RSSetViewports(1, m_viewPortShadowMap);

	ID3D11RenderTargetView* pRender[1] = { nullptr };
	context->OMSetRenderTargets(1, pRender, m_shadowMapDSView.Get());
}

void ShadowMap::SetRenderMode()
{
	context->ClearState();

	// ShaderResourceViewバインドを解除
	ID3D11ShaderResourceView* srv[] = { nullptr };
	context->PSSetShaderResources(
		1,              // 設定する最初のスロット番号
		1,              // 設定するシェーダ・リソース・ビューの数
		srv);			// 設定するシェーダ・リソース・ビューの配列

	// OMに描画ターゲット ビューと深度/ステンシル・ビューを設定
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// RSにビューポートを設定
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);
}

void ShadowMap::ApplyMode(bool shadowMode)
{
	// PSにサンプラーを設定
	ID3D11SamplerState* samplers[2] = { m_textureSampler[0].Get(), m_textureSampler[1].Get() };
	context->PSSetSamplers(0, 2, samplers);

	ID3D11ShaderResourceView* srv[] = { shadowMode ? nullptr : m_shadowMapSRView.Get() };
	context->PSSetShaderResources(
		1,              // 設定する最初のスロット番号
		1,              // 設定するシェーダ・リソース・ビューの数
		srv);			// 設定するシェーダ・リソース・ビューの配列

	// VSに頂点シェーダを設定
	context->VSSetShader(shadowMode ? m_vertexShaderShadow.Get() : m_vertexShader.Get(), nullptr, 0);

	// RSにラスタライザ・ステート・オブジェクトを設定
	context->RSSetState(shadowMode ? m_rasterizerStateShadow.Get() : m_rasterizerState.Get());

	// PSにピクセル・シェーダを設定
	context->PSSetShader(shadowMode ? nullptr : (m_shadowMappingEnabled ? m_pixelShader.Get() : m_pixelShaderNoSM.Get()), nullptr, 0);

	// VSに定数バッファを設定
	context->VSSetConstantBuffers(1, 1, g_pCBuffer.GetAddressOf());

	// PSに定数バッファを設定
	auto& light = GameContext::Get<Light>();
	m_constBuffer.SMViewProj = (light.view * light.projection).Transpose();
	context->UpdateSubresource(g_pCBuffer.Get(), 0, nullptr, &m_constBuffer, 0, 0);
	context->PSSetConstantBuffers(1, 1, g_pCBuffer.GetAddressOf());

	// OMにブレンド・ステート・オブジェクトを設定
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(m_blendState.Get(), BlendFactor, 0xffffffff);
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
}

void ShadowMap::ApplyShadowMode()
{
	ApplyMode(true);
}

void ShadowMap::ApplyRenderMode()
{
	ApplyMode(false);
}
