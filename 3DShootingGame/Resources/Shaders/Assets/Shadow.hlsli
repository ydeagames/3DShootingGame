#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"
#include "Lighting.fxh"
#include "Utilities.fxh"

cbuffer ShadowParameters : register(b1)
{
    float4x4 SMWorldViewProj;    // ワールド×ビュー×透視変換行列(シャドウマップ用)
};

// テクスチャ
Texture2D Texture : register(t0);
Texture2D ShadowMap : register(t1); // シャドウマップ

// テクスチャ・サンプラ
SamplerState Sampler   : register(s0);
SamplerState smpBorder : register(s1);
SamplerComparisonState smpCompare : register(s2);

// **************************************************
// 3Dオブジェクトの描画
// **************************************************

// 頂点シェーダの関数
VSOutputTxEnvMap VS(VSInputNmTx vin) {
    VSOutputTxEnvMap vout;

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    // 頂点座標　モデル座標系→透視座標系(シャドウマップ)
    float4 pos4 = vin.Position;
    pos4 = mul(pos4, SMWorldViewProj);
    pos4.xyz = pos4.xyz / pos4.w;
    vout.EnvCoord.x = (pos4.x + 1.0) / 2.0;
    vout.EnvCoord.y = (-pos4.y + 1.0) / 2.0;
    vout.EnvCoord.z = pos4.z;

    // 出力
    return vout;
}

// ピクセル シェーダの関数
float4 PS_NOSM(PSInputTxEnvMap pin) : SV_TARGET
{
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * pin.Diffuse;

    AddSpecular(color, pin.Specular.rgb);
    ApplyFog(color, pin.Specular.w);

    return color;
}

// ピクセル シェーダの関数
float4 PS(PSInputTxEnvMap pin) : SV_TARGET
{
    // テクスチャ取得
    float4 texCol = PS_NOSM(pin);

    // シャドウマップ
    //float sm = ShadowMap.Sample(smpBorder, pin.EnvCoord.xy).x;
    //float sma = (pin.EnvCoord.z < sm) ? 1.0 : 0.5;

	float3 shadowCoord = pin.EnvCoord;
	
	// 最大深度傾斜を求める.
	float  maxDepthSlope = max( abs( ddx( shadowCoord.z ) ), abs( ddy( shadowCoord.z ) ) );
	
	float  shadowThreshold = 1.0f;      // シャドウにするかどうかの閾値です.
	float  bias            = 0.001f;     // 固定バイアスです.
	float  slopeScaledBias = 1.0f;     // 深度傾斜.
	float  depthBiasClamp  = 1.0f;      // バイアスクランプ値.
	
	float  shadowBias = bias + slopeScaledBias * maxDepthSlope;
	shadowBias = min( shadowBias, depthBiasClamp );
	
	float shadowColor     = 0.25f;
	shadowThreshold = ShadowMap.SampleCmpLevelZero(smpCompare, shadowCoord.xy, shadowCoord.z - shadowBias );
	shadowColor     = lerp( shadowColor, 1.0f, shadowThreshold );

    // 色
    return saturate(texCol * shadowColor);
}

// **************************************************
// シャドウマップの描画
// **************************************************

// 頂点シェーダの関数
float4 VS_SM(VSInputNmTx vin) : SV_POSITION {
    // 頂点座標　モデル座標系→透視座標系
    return mul(vin.Position, WorldViewProj);
}

