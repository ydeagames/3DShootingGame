#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Shade.hlsli"

Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
SamplerState Sampler : register(s0);

float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;


	// Set the bias value for fixing the floating point precision issues.
	bias = 0.001f;

	// Set the default output color to the ambient light value for all pixels.
	color = float4(EmissiveColor, 1);

	// Calculate the projected texture coordinates.
	projectTexCoord.x = input.LightViewPosition.x / input.LightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.LightViewPosition.y / input.LightViewPosition.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample(Sampler, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.LightViewPosition.z / input.LightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if (lightDepthValue < depthValue)
		{
			// Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(input.Normal, input.LightPosition));

			if (lightIntensity > 0.0f)
			{
				// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
				color += (DiffuseColor * lightIntensity);

				// Saturate the final light color.
				color = saturate(color);
			}
		}
	}

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(Sampler, input.TexCoord);

	// Combine the light and texture color.
	color = color * textureColor;

	return color;
}