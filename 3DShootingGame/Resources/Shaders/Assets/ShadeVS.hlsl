#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"

VSOutputTx VSBasicVc(VSInputNmTx vin)
{
    VSOutputTx vout;

    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParams;

	// Transform the vertex position into projected space from the POV of the light.
	float4 modelPos = mul(vin.Position, World);
	float4 lightSpacePos = mul(modelPos, LightView);
	lightSpacePos = mul(lightSpacePos, LightProjection);
	vout.lightSpacePos = lightSpacePos;

	// Light ray
	float3 lRay = LightPosition.xyz - modelPos.xyz;
	vout.lRay = lRay;

	// Camera ray
	vout.view = EyePosition.xyz - modelPos.xyz;

	// Transform the vertex normal into world space.
	float4 norm = float4(input.norm, 1.0f);
	norm = mul(norm, model);
	vout.norm = norm.xyz;

	// Pass through the color and texture coordinates without modification.
	vout.TexCoord = vin.TexCoord;

	return vout;
}
