#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"
#include "Shade.hlsli"
#include "Lighting.fxh"

VSOutputShadowTx VSBasicVc(VSInputNmTx vin)
{
	VSOutputShadowTx vout;

	CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
	SetCommonVSOutputParams;

	// Transform the vertex position into projected space from the POV of the light.
	float4 modelPos = mul(vin.Position, World);
	float4 lightSpacePos = mul(modelPos, LightView);
	lightSpacePos = mul(lightSpacePos, LightProjection);
	vout.lightSpacePos = lightSpacePos;

	// Camera ray
	vout.view = EyePosition.xyz - modelPos.xyz;

	// Transform the vertex normal into world space.
	float4 norm = float4(vin.Normal, 1.0f);
	norm = mul(norm, World);
	vout.Normal = norm.xyz;

	// Pass through the color and texture coordinates without modification.
	vout.TexCoord = vin.TexCoord;

	return vout;
}
