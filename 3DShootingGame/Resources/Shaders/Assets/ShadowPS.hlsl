#include "Structures.fxh"
#include "Shadow.hlsli"

float4 DepthPixelShader(PixelInputType input) : SV_TARGET
{
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	float depthValue = input.PositionPS.z / input.PositionPS.w;

	float4 color = float4(depthValue, depthValue, depthValue, 1.0f);

	return color;
}
