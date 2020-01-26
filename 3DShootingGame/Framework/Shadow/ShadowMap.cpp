#include "pch.h"
#include "ShadowMap.h"
#include "Framework/ECS/GameContext.h"
#include "Utilities/BinaryFile.h"
#include "Framework/Context/GameCamera.h"
#include "Utilities/wavefrontobj.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void ShadowMap::CreateShaderObj()
{
	auto g_pD3DDevice = GameContext::Get<DX::DeviceResources>().GetD3DDevice();

	// **********************************************************
	// 頂点シェーダのコードをロード
	BinaryFile pBlobVS = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_VS.cso");
	// 頂点シェーダの作成
	DX::ThrowIfFailed(g_pD3DDevice->CreateVertexShader(
		pBlobVS.GetData(), // バイト・コードへのポインタ
		pBlobVS.GetSize(),    // バイト・コードの長さ
		NULL,
		g_pVertexShader.ReleaseAndGetAddressOf())); // 頂点シェーダを受け取る変数

	// 入力レイアウト・オブジェクトの作成
	DX::ThrowIfFailed(g_pD3DDevice->CreateInputLayout(
		VertexPositionNormalTexture::InputElements,                            // 定義の配列
		UINT(VertexPositionNormalTexture::InputElementCount),                  // 定義の要素数
		pBlobVS.GetData(),       // バイト・コードへのポインタ
		pBlobVS.GetSize(),          // バイト・コードのサイズ
		g_pInputLayout.ReleaseAndGetAddressOf()));                  // 受け取る変数のポインタ

	// **********************************************************
	// ピクセル・シェーダのコードをコンパイル
	BinaryFile pBlobPS = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_PS.cso");
	// ピクセル・シェーダの作成
	DX::ThrowIfFailed(g_pD3DDevice->CreatePixelShader(
		pBlobPS.GetData(), // バイト・コードへのポインタ
		pBlobPS.GetSize(),    // バイト・コードの長さ
		NULL,
		g_pPixelShader.ReleaseAndGetAddressOf())); // ピクセル・シェーダを受け取る変数

	// ピクセル・シェーダのコードをコンパイル
	BinaryFile pBlobPSNOSM = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_PS_NOSM.cso");
	// ピクセル・シェーダの作成
	DX::ThrowIfFailed(g_pD3DDevice->CreatePixelShader(
		pBlobPSNOSM.GetData(), // バイト・コードへのポインタ
		pBlobPSNOSM.GetSize(),    // バイト・コードの長さ
		NULL,
		g_pPixelShaderNoSM.ReleaseAndGetAddressOf())); // ピクセル・シェーダを受け取る変数

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
	DX::ThrowIfFailed(g_pD3DDevice->CreateRasterizerState(&RSDesc, g_pRasterizerState.GetAddressOf()));
}

void ShadowMap::CreateShaderShadow()
{
	auto g_pD3DDevice = GameContext::Get<DX::DeviceResources>().GetD3DDevice();

	// **********************************************************
	// 頂点シェーダのコードをコンパイル
	BinaryFile pBlobVS = BinaryFile::LoadFile(L"Resources/Shaders/Shadow_VS_SM.cso");
	// 頂点シェーダの作成
	DX::ThrowIfFailed(g_pD3DDevice->CreateVertexShader(
		pBlobVS.GetData(), // バイト・コードへのポインタ
		pBlobVS.GetSize(),    // バイト・コードの長さ
		NULL,
		g_pVertexShaderShadow.ReleaseAndGetAddressOf())); // 頂点シェーダを受け取る変数

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
	DX::ThrowIfFailed(g_pD3DDevice->CreateRasterizerState(&RSDesc, g_pRasterizerStateShadow.GetAddressOf()));
}

void ShadowMap::InitBackBuffer()
{
	auto& dr = GameContext::Get<DX::DeviceResources>();

	// スワップ・チェインから最初のバック・バッファを取得する
	ID3D11Texture2D* pBackBuffer = dr.GetRenderTarget();  // バッファのアクセスに使うインターフェイス

	// バック・バッファの情報
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	// バック・バッファの描画ターゲット・ビューを作る
	g_pRenderTargetView = dr.GetRenderTargetView();

	// 深度/ステンシル・テクスチャの作成
	g_pDepthStencil = dr.GetDepthStencil();

	// 深度/ステンシル ビューの作成
	g_pDepthStencilView = dr.GetDepthStencilView();

	// ビューポートの設定
	g_ViewPort[0] = dr.GetScreenViewport();

	// 定数バッファを更新
	// 射影変換行列(パースペクティブ(透視法)射影)
	g_cbCBuffer.Projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(30.0f),		// 視野角30°
		(float)descBackBuffer.Width / (float)descBackBuffer.Height,	// アスペクト比
		1.0f,							// 前方投影面までの距離
		400.0f);						// 後方投影面までの距離

	//サイズを保存
	g_sizeWindow.cx = descBackBuffer.Width;
	g_sizeWindow.cy = descBackBuffer.Height;
}

void ShadowMap::InitShadowMap()
{
	auto g_pD3DDevice = GameContext::Get<DX::DeviceResources>().GetD3DDevice();

	// シャドウ マップの作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = 512;   // 幅
	descDepth.Height = 512;  // 高さ
	descDepth.MipLevels = 1;       // ミップマップ レベル数
	descDepth.ArraySize = 1;       // 配列サイズ
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;  // フォーマット
	descDepth.SampleDesc.Count = 1;  // マルチサンプリングの設定
	descDepth.SampleDesc.Quality = 0;  // マルチサンプリングの品質
	descDepth.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // 深度/ステンシル、シェーダ リソース ビューとして使用
	descDepth.CPUAccessFlags = 0;   // CPUからはアクセスしない
	descDepth.MiscFlags = 0;   // その他の設定なし
	DX::ThrowIfFailed(g_pD3DDevice->CreateTexture2D(
		&descDepth,         // 作成する2Dテクスチャの設定
		NULL,               // 
		g_pShadowMap.ReleaseAndGetAddressOf()));     // 作成したテクスチャを受け取る変数

	// 深度/ステンシル ビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;            // ビューのフォーマット
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	DX::ThrowIfFailed(g_pD3DDevice->CreateDepthStencilView(
		g_pShadowMap.Get(),         // 深度/ステンシル・ビューを作るテクスチャ
		&descDSV,             // 深度/ステンシル・ビューの設定
		g_pShadowMapDSView.ReleaseAndGetAddressOf())); // 作成したビューを受け取る変数

	// シェーダ リソース ビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT; // フォーマット
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
	srDesc.Texture2D.MostDetailedMip = 0;   // 最初のミップマップ レベル
	srDesc.Texture2D.MipLevels = -1;  // すべてのミップマップ レベル
	DX::ThrowIfFailed(g_pD3DDevice->CreateShaderResourceView(
		g_pShadowMap.Get(),          // アクセスするテクスチャ リソース
		&srDesc,               // シェーダ リソース ビューの設定
		g_pShadowMapSRView.ReleaseAndGetAddressOf()));  // 受け取る変数

	// ビューポートの設定
	g_ViewPortShadowMap[0].TopLeftX = 0.0f;		// ビューポート領域の左上X座標。
	g_ViewPortShadowMap[0].TopLeftY = 0.0f;		// ビューポート領域の左上Y座標。
	g_ViewPortShadowMap[0].Width = 512.0f;	// ビューポート領域の幅
	g_ViewPortShadowMap[0].Height = 512.0f;	// ビューポート領域の高さ
	g_ViewPortShadowMap[0].MinDepth = 0.0f;		// ビューポート領域の深度値の最小値
	g_ViewPortShadowMap[0].MaxDepth = 1.0f;		// ビューポート領域の深度値の最大値
}

namespace DirectX
{
	typedef std::vector<DirectX::VertexPositionNormalTexture> VertexCollection;
	typedef std::vector<uint16_t> IndexCollection;

	void ComputeBox(VertexCollection& vertices, IndexCollection& indices, const XMFLOAT3& size, bool rhcoords, bool invertn);
	void ComputeTeapot(VertexCollection& vertices, IndexCollection& indices, float size, size_t tessellation, bool rhcoords);
}

void ShadowMap::RenderStart()
{
	auto g_pD3DDevice = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto g_pImmediateContext = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

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
	cBufferDesc.ByteWidth = sizeof(cbCBuffer); // バッファ・サイズ
	DX::ThrowIfFailed(g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, g_pCBuffer.ReleaseAndGetAddressOf()));

	// **********************************************************
	// ブレンド・ステート・オブジェクトの作成
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	DX::ThrowIfFailed(g_pD3DDevice->CreateBlendState(&BlendState, g_pBlendState.ReleaseAndGetAddressOf()));

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
	DX::ThrowIfFailed(g_pD3DDevice->CreateDepthStencilState(&DepthStencil, &g_pDepthStencilState));

	// **********************************************************
	// Waveform OBJファイルの読み込み
	char mtlFileName[80];
	DX::ThrowIfFailed(g_wfObjKuma.Load(g_pD3DDevice, g_pImmediateContext, "Resources/Models/kuma.obj", mtlFileName, sizeof(mtlFileName)));
	// MTLファイルの読み込み
	DX::ThrowIfFailed(g_wfMtl.Load(g_pD3DDevice, g_pImmediateContext, mtlFileName, "Resources/Models/default.bmp"));

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
	DX::ThrowIfFailed(g_pD3DDevice->CreateSamplerState(&descSampler, &g_pTextureSampler[0]));

	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.BorderColor[0] = 1.0f;
	descSampler.BorderColor[1] = 1.0f;
	descSampler.BorderColor[2] = 1.0f;
	descSampler.BorderColor[3] = 1.0f;
	DX::ThrowIfFailed(g_pD3DDevice->CreateSamplerState(&descSampler, &g_pTextureSampler[1]));

	// **********************************************************
	// バック バッファの初期化
	InitBackBuffer();

	// **********************************************************
	// シャドウ・マップの作成
	InitShadowMap();

	// モデル
	CreateWICTextureFromFile(g_pD3DDevice, L"Resources/Textures/GridBox_Default.png", nullptr, modelTexture.ReleaseAndGetAddressOf());

	primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionNormalTexture>>(g_pImmediateContext, 2048 * 10, 2048 * 2);
	DirectX::ComputeBox(modelVertices, modelIndices, Vector3::One, true, false);
	auto model2Vertices = modelVertices;
	auto model2Indices = modelIndices;
	for (auto& value : model2Vertices)
		value.position = value.position + Vector3(1, 1, -1);
	for (auto& value : model2Indices)
		value += uint16_t(modelVertices.size());
	modelVertices.insert(modelVertices.end(), model2Vertices.begin(), model2Vertices.end());
	modelIndices.insert(modelIndices.end(), model2Indices.begin(), model2Indices.end());
	// DirectX::ComputeTeapot(modelVertices, modelIndices, 1, 8, true);
}

void ShadowMap::Begin()
{
}

/*--------------------------------------------
	Objの描画処理
--------------------------------------------*/
void ShadowMap::DrawObj(bool drawShadowMap)
{
	auto g_pImmediateContext = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	WFOBJ_GROUP* pGroup = g_wfObjKuma.GetGroup();
	int countGroup = g_wfObjKuma.GetCountGroup();
	for (int g = 0; g < countGroup; ++g) {
		if (pGroup[g].countIndex <= 0)
			continue;

		// マテリアル設定
		WFOBJ_MTL* pMtl = g_wfMtl.GetMaterial(pGroup[g].mtl);
		ID3D11ShaderResourceView* srv[2];
		srv[0] = g_wfMtl.GetTextureDefault(pMtl->map_Kd);
		srv[1] = drawShadowMap ? NULL : g_pShadowMapSRView.Get();

		g_cbCBuffer.Diffuse.x = pMtl->Kd[0];
		g_cbCBuffer.Diffuse.y = pMtl->Kd[1];
		g_cbCBuffer.Diffuse.z = pMtl->Kd[2];
		g_cbCBuffer.Diffuse.w = pMtl->d;

		// 定数バッファの内容更新
		g_pImmediateContext->UpdateSubresource(g_pCBuffer.Get(), 0, NULL, &g_cbCBuffer, 0, 0);

		// PSにシェーダ・リソース・ビューを設定
		g_pImmediateContext->PSSetShaderResources(
			0,              // 設定する最初のスロット番号
			2,              // 設定するシェーダ・リソース・ビューの数
			srv);			// 設定するシェーダ・リソース・ビューの配列

		// メッシュを描画
		g_wfObjKuma.Draw(g);
	}
}

/*--------------------------------------------
	物体の描画処理
--------------------------------------------*/
void ShadowMap::RenderObj(bool drawShadowMap)
{
	auto g_pImmediateContext = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();
	// **********************************************************
	// IAにOBJファイルの頂点バッファ/インデックス・バッファを設定
	g_wfObjKuma.SetIA();
	// IAに入力レイアウト・オブジェクトを設定
	g_pImmediateContext->IASetInputLayout(g_pInputLayout.Get());

	// VSに頂点シェーダを設定
	g_pImmediateContext->VSSetShader(drawShadowMap ? g_pVertexShaderShadow.Get() : g_pVertexShader.Get(), NULL, 0);

	// RSにラスタライザ・ステート・オブジェクトを設定
	g_pImmediateContext->RSSetState(drawShadowMap ? g_pRasterizerStateShadow.Get() : g_pRasterizerState.Get());

	// PSにピクセル・シェーダを設定
	g_pImmediateContext->PSSetShader(drawShadowMap ? NULL : (g_bShadowMappingMode ? g_pPixelShader.Get() : g_pPixelShaderNoSM.Get()), NULL, 0);

	// OMにブレンド・ステート・オブジェクトを設定
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_pImmediateContext->OMSetBlendState(g_pBlendState.Get(), BlendFactor, 0xffffffff);
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState.Get(), 0);

	// ***************************************
	// 3Dオブジェクトを描画
	DrawObj(drawShadowMap);
}

/*--------------------------------------------
	画面の描画処理
--------------------------------------------*/
void ShadowMap::Render(GameCamera& camera)
{
	auto g_pImmediateContext = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// 定数バッファを更新
	// ワールド変換行列
	FLOAT rotate = (FLOAT)(XM_PI * GameContext::Get<DX::StepTimer>().GetTotalSeconds()) / 15.0f;
	XMMATRIX matWorld = Matrix::CreateRotationY(rotate);
	XMStoreFloat4x4(&g_cbCBuffer.World, XMMatrixTranspose(matWorld));

	// ***************************************
	// シャドウ マップの描画
	if (g_bShadowMappingMode) {
		g_pImmediateContext->ClearState();

		// ビュー変換行列(光源から見る)
		XMVECTORF32 focusPosition = { 0.0f, 0.0f,  0.0f };  // 注視点
		XMVECTORF32 upDirection = { 0.0f, 1.0f,  0.0f };  // カメラの上方向
		XMMATRIX matShadowMapView = XMMatrixLookAtLH(XMLoadFloat3(&g_vLightPos), focusPosition, upDirection);
		XMStoreFloat4x4(&g_cbCBuffer.View, XMMatrixTranspose(matShadowMapView));

		// 射影変換行列(パースペクティブ(透視法)射影)
		XMMATRIX matShadowMapProj = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),		// 視野角45°
			g_ViewPortShadowMap[0].Width / g_ViewPortShadowMap[0].Height,	// アスペクト比
			1.0f,							// 前方投影面までの距離
			400.0f);						// 後方投影面までの距離
		XMStoreFloat4x4(&g_cbCBuffer.Projection, XMMatrixTranspose(matShadowMapProj));

		// 深度/ステンシルのクリア
		g_pImmediateContext->ClearDepthStencilView(g_pShadowMapDSView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		// VSに定数バッファを設定
		g_pImmediateContext->VSSetConstantBuffers(0, 1, g_pCBuffer.GetAddressOf());

		// PSに定数バッファを設定
		g_pImmediateContext->PSSetConstantBuffers(0, 1, g_pCBuffer.GetAddressOf());
		// PSにサンプラーを設定
		ID3D11SamplerState* samplers[2] = { g_pTextureSampler[0].Get(), g_pTextureSampler[1].Get() };
		g_pImmediateContext->PSSetSamplers(0, 2, samplers);

		// RSにビューポートを設定
		g_pImmediateContext->RSSetViewports(1, g_ViewPortShadowMap);

		// OMに描画ターゲット ビューと深度/ステンシル・ビューを設定
		ID3D11RenderTargetView* pRender[1] = { NULL };
		g_pImmediateContext->OMSetRenderTargets(1, pRender, g_pShadowMapDSView.Get());

		// 物体の描画
		RenderObj(true);

		// シャドウマップの設定
		XMMATRIX mat = XMMatrixTranspose(matWorld * matShadowMapView * matShadowMapProj);
		XMStoreFloat4x4(&g_cbCBuffer.SMWorldViewProj, mat);
	}

	// ***************************************
	// ビュー変換行列
	Matrix matView = camera.view;
	XMStoreFloat4x4(&g_cbCBuffer.View, XMMatrixTranspose(matView));
	// 射影変換行列(パースペクティブ(透視法)射影)
	XMMATRIX matProj = camera.projection;
	XMStoreFloat4x4(&g_cbCBuffer.Projection, XMMatrixTranspose(matProj));
	// 点光源座標
	XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&g_vLightPos), matView);
	XMStoreFloat3(&g_cbCBuffer.Light, vec);

	//// ***************************************
	//// 描画ターゲットのクリア
	//g_pImmediateContext->ClearRenderTargetView(
	//	g_pRenderTargetView, // クリアする描画ターゲット
	//	g_ClearColor);         // クリアする値

// 深度/ステンシルのクリア
	g_pImmediateContext->ClearDepthStencilView(
		g_pDepthStencilView, // クリアする深度/ステンシル・ビュー
		D3D11_CLEAR_DEPTH,   // 深度値だけをクリアする
		1.0f,                // 深度バッファをクリアする値
		0);                  // ステンシル・バッファをクリアする値(この場合、無関係)

// ***************************************
	g_pImmediateContext->ClearState();

	// VSに定数バッファを設定
	g_pImmediateContext->VSSetConstantBuffers(0, 1, g_pCBuffer.GetAddressOf());

	// PSに定数バッファを設定
	g_pImmediateContext->PSSetConstantBuffers(0, 1, g_pCBuffer.GetAddressOf());
	// PSにサンプラーを設定
	ID3D11SamplerState* samplers[2] = { g_pTextureSampler[0].Get(), g_pTextureSampler[1].Get() };
	g_pImmediateContext->PSSetSamplers(0, 2, samplers);

	// RSにビューポートを設定
	g_pImmediateContext->RSSetViewports(1, g_ViewPort);

	// OMに描画ターゲット ビューと深度/ステンシル・ビューを設定
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_bDepthMode ? g_pDepthStencilView : NULL);

	// 物体の描画
	RenderObj(false);

	// ***************************************
	//// バック バッファの表示
	//hr = g_pSwapChain->Present(0,	// 画面を直ぐに更新する
	//	0);	// 画面を実際に更新する

	//return hr;
}

void ShadowMap::End()
{
}
