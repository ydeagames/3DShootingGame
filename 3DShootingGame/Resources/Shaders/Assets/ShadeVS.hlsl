#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"
#include "Shade.hlsli"
#include "Lighting.fxh"

/*
VSOutputShadow VSBasicVc(VSInputNm vin)
{
	VSOutputShadow vout;

	CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
	SetCommonVSOutputParams;

	// Transform the vertex position into projected space from the POV of the light.
	float4 modelPos = mul(vin.Position, World);
	float4 lightSpacePos = mul(modelPos, LightView);
	lightSpacePos = mul(lightSpacePos, LightProjection);
	vout.LightViewPosition = lightSpacePos;

	// Camera ray
	vout.LightPos = EyePosition.xyz - modelPos.xyz;

	// Transform the vertex normal into world space.
	float4 norm = float4(vin.Normal, 1.0f);
	norm = mul(norm, World);
	vout.Normal = norm.xyz;

	// Pass through the color and texture coordinates without modification.
	//vout.TexCoord = vin.TexCoord;

	return vout;
}
*/

PixelInputType ShadowVertexShader(VSInputNmTx vin)
{
	PixelInputType vout;

	CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
	SetCommonVSOutputParams;

	float4 worldPosition;

	//// Change the position vector to be 4 units for proper matrix calculations.
	//vin.Position.w = 1.0f;

	// Calculate the position of the vertice as viewed by the light source.
	vout.LightViewPosition = mul(vin.Position, World);
	vout.LightViewPosition = mul(vout.LightViewPosition, LightViewProj);

	// Store the texture coordinates for the pixel shader.
	vout.TexCoord = vin.TexCoord;

	// Calculate the normal vector against the world matrix only.
	vout.Normal = mul(vin.Normal, (float3x3)World);

	// Normalize the normal vector.
	vout.Normal = normalize(vout.Normal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(vin.Position, World);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	vout.LightPosition = LightPosition.xyz - worldPosition.xyz;

	// Normalize the light position vector.
	vout.LightPosition = normalize(vout.LightPosition);

	return vout;
}