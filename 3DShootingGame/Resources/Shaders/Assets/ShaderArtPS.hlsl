#include "Parameter.hlsli"
#include "Structures.fxh"

Texture2D<float4> Texture : register(t0);
Texture2D<float4> _MaskTex : register(t1);
sampler Sampler : register(s0);

float4 sampleGradation(float2 uv)
{
	float col = 1 - uv.y;
	return float4(col, col, col, 1);
}

float4 PSBasicTx(PSInputTx input) : SV_TARGET
{
	//float4 mainTex = Texture.Sample(Sampler, input.TexCoord);

	float2 uv = input.TexCoord;

	//uv = float2(uv.x, uv.y * 3.f % 1.f);

	float line1 = .5f;
	float line2 = .75f;

	float t = Time.x;
	line1 += (sin(uv.x * 3.2 + t) * .04) + (sin(uv.x * 5.3 + t) * .02) + (sin(uv.x * 8.2 + t) * .008);
	line2 += (sin(uv.x * 8 + t) * .05) + (sin(uv.x * 2.3 + t) * .05) + (sin(uv.x * 4.2 + t) * .009);

	if (uv.y > line2)
		uv.y -= 0.1;
	if (uv.y > line1)
		uv.y -= 0.1;

	float4 grayscale = sampleGradation(uv);
	grayscale *= float4(1, .7, .7, 1);
	return grayscale;
}