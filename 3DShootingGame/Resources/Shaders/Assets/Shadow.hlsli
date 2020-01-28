cbuffer Parameters : register(b0)
{
    float4 DiffuseColor0             : packoffset(c0);
    float3 EmissiveColor            : packoffset(c1);
    float3 SpecularColor            : packoffset(c2);
    float  SpecularPower : packoffset(c2.w);

    float3 LightDirection[3]        : packoffset(c3);
    float3 LightDiffuseColor[3]     : packoffset(c6);
    float3 LightSpecularColor[3]    : packoffset(c9);

    float3 EyePosition              : packoffset(c12);

    float3 FogColor                 : packoffset(c13);
    float4 FogVector                : packoffset(c14);

    float4x4 World                  : packoffset(c15);
    float3x3 WorldInverseTranspose  : packoffset(c19);
    float4x4 WorldViewProj          : packoffset(c22);
};

cbuffer ShadowParameters : register(b1)
{
    float4x4 View;          // ビュー変換行列
    float4x4 Projection;    // 透視変換行列
    float4x4 SMViewProj;    // ワールド×ビュー×透視変換行列(シャドウマップ用)
    float3 Light;           // 光源座標(透視座標系)
    float dummy;
    float4x4 World0;
    float4 DiffuseColor;
};

// テクスチャ
Texture2D Tex2D : register(t0);
Texture2D ShadowMap : register(t1); // シャドウマップ

// テクスチャ・サンプラ
SamplerState smpWrap   : register(s0);
SamplerState smpBorder : register(s1);

// **************************************************
// 3Dオブジェクトの描画
// **************************************************

// 頂点シェーダの入力データ定義
struct VSInputNmTx {
    float3 Position  : SV_Position;  // 頂点座標(モデル座標系)
    float3 Normal : NORMAL;    // 法線ベクトル(モデル座標系)
    float2 TexCoord  : TEXCOORD;   // テクスチャ座標
};

// ピクセル シェーダの入力データ定義
struct VSOutputPixelLightingTx {
    float2 TexCoord     : TEXCOORD0;    // テクスチャ座標
    float4 PositionWS   : TEXCOORD1;    // 頂点座標(ビュー座標系)
    float3 NormalWS     : TEXCOORD2;    // 法線ベクトル(ビュー座標系)
    float4 Diffuse      : COLOR0;
    float4 PositionPS   : SV_POSITION;  // 頂点座標(透視座標系)
    float3 PositionSM   : TEXCOORD3;    // 頂点座標(シャドウマップの透視座標系)
};
struct PSInputPixelLightingTx {
    float2 TexCoord     : TEXCOORD0;    // テクスチャ座標
    float4 PositionWS   : TEXCOORD1;    // 頂点座標(ビュー座標系)
    float3 NormalWS     : TEXCOORD2;    // 法線ベクトル(ビュー座標系)
    float4 Diffuse      : COLOR0;
    float4 PositionPS   : SV_POSITION;  // 頂点座標(透視座標系)
    float3 PositionSM   : TEXCOORD3;    // 頂点座標(シャドウマップの透視座標系)
};
/*
struct VSOutputPixelLightingTx
{
    float2 TexCoord   : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS   : TEXCOORD2;
    float4 Diffuse    : COLOR0;
    float4 PositionPS : SV_Position;
};
*/

// 頂点シェーダの関数
VSOutputPixelLightingTx VS(VSInputNmTx vin) {
    VSOutputPixelLightingTx vout;

    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(vin.Position, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    vout.PositionWS = float4(pos4.xyz / pos4.w, 1);
	pos4 = mul(pos4, Projection);
	vout.PositionPS = pos4;

    // 法線ベクトル　モデル座標系→ビュー座標系
    float3 Normal = mul(vin.Normal, (float3x3)World);
    vout.NormalWS = mul(Normal, (float3x3)View);

    // テクスチャ座標
    vout.TexCoord = vin.TexCoord;

    // 頂点座標　モデル座標系→透視座標系(シャドウマップ)
    pos4 = float4(vin.Position, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, SMViewProj);
    pos4.xyz = pos4.xyz / pos4.w;
    vout.PositionSM.x = (pos4.x + 1.0) / 2.0;
    vout.PositionSM.y = (-pos4.y + 1.0) / 2.0;
    vout.PositionSM.z = pos4.z;

    // 出力
    return vout;
}

// ライティング計算
float lighting(PSInputPixelLightingTx pin)
{
    // 光源ベクトル
    float3 light = Light - pin.PositionWS;
    // 距離
    float  leng = length(light);
    // 明るさ
    return 30 * dot(normalize(light), normalize(pin.NormalWS)) / pow(leng, 2);
}

// ピクセル シェーダの関数
float4 PS(PSInputPixelLightingTx pin) : SV_TARGET
{
    //return float4(World0._11,World0._22,World0._33,1);
    //return float4(World._11,World._22,World._33,1);
    // ライティング計算
    float bright = lighting(pin);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, pin.TexCoord);         // テクセル読み込み

    // シャドウマップ
    float sm = ShadowMap.Sample(smpBorder, pin.PositionSM.xy).x;
    float sma = (pin.PositionSM.z < sm) ? 1.0 : 0.5;

    // 色
    return saturate(bright * texCol * DiffuseColor * sma);
}

// **************************************************
// シャドウマップを使わない3Dオブジェクトの描画
// **************************************************

// ピクセル シェーダの関数
float4 PS_NOSM(PSInputPixelLightingTx pin) : SV_TARGET
{
    // ライティング計算
    float bright = lighting(pin);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, pin.TexCoord);         // テクセル読み込み

    // 色
    return saturate(bright * texCol * DiffuseColor);
}

// **************************************************
// シャドウマップの描画
// **************************************************

// 頂点シェーダの関数
float4 VS_SM(VSInputNmTx vin) : SV_POSITION {
    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(vin.Position, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    return mul(pos4, Projection);
}

