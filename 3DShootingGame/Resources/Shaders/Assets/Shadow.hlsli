#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"
#include "Lighting.fxh"

cbuffer ShadowParameters : register(b1)
{
    float4x4 View;          // ビュー変換行列
    float4x4 Projection;    // 透視変換行列
    float4x4 SMViewProj;    // ワールド×ビュー×透視変換行列(シャドウマップ用)
    float3 Light;           // 光源座標(透視座標系)
    float dummy;
    float4x4 World0;
    float4 DiffuseColor0;
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

// ピクセル シェーダの入力データ定義
struct VSOutputPixelLightingTxSM : VSOutputPixelLightingTx {
    float3 PositionSM   : TEXCOORD3;    // 頂点座標(シャドウマップの透視座標系)
};
struct PSInputPixelLightingTxSM : VSOutputPixelLightingTxSM {
};

// 頂点シェーダの関数
VSOutputPixelLightingTxSM VS(VSInputNmTx vin) {
    VSOutputPixelLightingTxSM vout;

    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = vin.Position;
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
    pos4 = vin.Position;
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
float lighting(PSInputPixelLightingTxSM pin)
{
    // 光源ベクトル
    float3 light = Light - pin.PositionWS.xyz;
    // 距離
    float  leng = length(light);
    // 明るさ
    return 30 * dot(normalize(light), normalize(pin.NormalWS)) / pow(leng, 2);
}

// ピクセル シェーダの関数
float4 PS(PSInputPixelLightingTxSM pin) : SV_TARGET
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
float4 PS_NOSM(PSInputPixelLightingTxSM pin) : SV_TARGET
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
    float4 pos4 = vin.Position;
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    return mul(pos4, Projection);
}

