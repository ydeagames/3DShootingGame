#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"
#include "Lighting.fxh"

#define SAMPLES_COUNT 32
#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)
#define SHADOW_COLOR 0.25f;

cbuffer ShadowParameters : register(b1)
{
	float4x4 SMViewProj;    // ワールド×ビュー×透視変換行列(シャドウマップ用)
};

// テクスチャ
Texture2D Texture : register(t0);
Texture2D ShadowMap : register(t1); // シャドウマップ

// テクスチャ・サンプラ
SamplerState Sampler   : register(s0);
SamplerComparisonState smpCompare : register(s1);

// **************************************************
// 3Dオブジェクトの描画
// **************************************************

// 頂点シェーダの関数
VSOutputTxEnvMap VS(VSInputNmTx vin)
{
	VSOutputTxEnvMap vout;

	CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
	SetCommonVSOutputParams;

	vout.TexCoord = vin.TexCoord;

	// 頂点座標　モデル座標系→透視座標系(シャドウマップ)
	float4 pos4 = vin.Position;
	pos4 = mul(pos4, World);
	pos4 = mul(pos4, SMViewProj);
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

float multiSample(float2 coord, float cmp)
{
	float shadowThreshold = 0;
	float2 smCoord = coord;
	float fsize = 4;
	for (int i = 0; i < SAMPLES_COUNT; i++)
	{
		smCoord = offsets[i] * fsize + coord;
		shadow += texture2DProj(shadowMap, smCoord) * INV_SAMPLES_COUNT;
	}
}

// ピクセル シェーダの関数
float4 PS(PSInputTxEnvMap pin) : SV_TARGET
{
	// テクスチャ取得
	float4 texCol = PS_NOSM(pin);

	float3 shadowCoord = pin.EnvCoord;

	// 最大深度傾斜を求める.
	float  maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));

	float  shadowThreshold = 1.0f;      // シャドウにするかどうかの閾値です.
	float  bias = 0.001f;     // 固定バイアスです.
	float  slopeScaledBias = 1.0f;     // 深度傾斜.
	float  depthBiasClamp = 1.0f;      // バイアスクランプ値.

	float  shadowBias = bias + slopeScaledBias * maxDepthSlope;
	shadowBias = min(shadowBias, depthBiasClamp);

	shadowThreshold = ShadowMap.SampleCmpLevelZero(smpCompare, shadowCoord.xy, shadowCoord.z - shadowBias);
	shadowColor = lerp(SHADOW_COLOR, 1.0f, shadowThreshold);

	// 色
	return saturate(min(texCol, shadowColor));
}

// **************************************************
// シャドウマップの描画
// **************************************************

// 頂点シェーダの関数
float4 VS_SM(VSInputNmTx vin) : SV_POSITION
{
	// 頂点座標　モデル座標系→透視座標系
	float4 pos4 = vin.Position;
	pos4 = mul(pos4, World);
	pos4 = mul(pos4, SMViewProj);
	return pos4;
}

uniform sampler2D decal;
// decal texture
uniform sampler2D spot;
// projected spotlight image
uniform sampler2DShadow shadowMap;
// shadow map
uniform float fwidth;
uniform vec2 offsets[SAMPLES_COUNT];
// these are passed down from vertex shader
varying vec4 shadowMapPos;
varying vec3 normal;
varying vec2 texCoord;
varying vec3 lightVec;
varying vec3 view;

void main(void)
{
	float shadow = 0;
	float fsize = shadowMapPos.w * fwidth;
	vec4 smCoord = shadowMapPos;

	for (int i = 0; i < SAMPLES_COUNT; i++)
	{
		smCoord.xy = offsets[i] * fsize + shadowMapPos;
		shadow += texture2DProj(shadowMap, smCoord) * INV_SAMPLES_COUNT;
	}

	vec3 N = normalize(normal);
	vec3 L = normalize(lightVec);
	vec3 V = normalize(view);
	vec3 R = reflect(-V, N);

	// calculate diffuse dot product
	float NdotL = max(dot(N, L), 0);
	// modulate lighting with the computed shadow value
	vec3 color = texture2D(decal, texCoord).xyz;
	gl_FragColor.xyz = (color * NdotL + pow(max(dot(R, L), 0), 64)) *
		shadow * texture2DProj(spot, shadowMapPos) +
		color * 0.1;
}