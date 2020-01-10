#include "ShaderArt.fxh"
#include "Structures.fxh"

Texture2D<float4> _MainTex : register(t0);
Texture2D<float4> _MaskTex : register(t1);
SamplerState _SamLinear : register(s0);

VSOutputTx VSBasicVc(VSInputTx input)
{
	VSOutputTx output;
	output.TexCoord = input.TexCoord;
	output.PositionPS = input.Position;
	return output;
}

float4 PSBasicTx(PSInputTx input) : COLOR
{
	float4 mainTex = _MainTex.Sample(_SamLinear, input.TexCoord);
	
	float4 maskTex = _MaskTex.Sample(_SamLinear, input.TexCoord);
	float grayscale = maskTex.r;

	float range = Time.x;
	float2 mask;
	if (range < 1)
	{
		float range2 = 1-(range);
		mask = grayscale - (-1 + range2);
		clip(mask - 1.f);
	}
	else
	{
		float range2 = 1-(range-1);
		mask = grayscale - (-1 + range2);
		clip(-(mask - 1.f));
	}
	
	return mainTex;
}