#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Shade.hlsli"

Texture2D shadowMap : register(t2);
SamplerComparisonState	shadowSampler	: register(s8);

float3 DplusS(float3 N, float3 L, float NdotL, float3 view)
{
    const float3 Kdiffuse = float3(.5f, .5f, .4f);
    const float3 Kspecular = float3(.2f, .2f, .3f);
    const float exponent = 3.f;

    // Compute the diffuse coefficient.
    float diffuseConst = saturate(NdotL);

    // Compute the diffuse lighting value.
    float3 diffuse = Kdiffuse * diffuseConst;

    // Compute the specular highlight.
    float3 R = reflect(-L, N);
    float3 V = normalize(view);
    float3 RdotV = dot(R, V);
    float3 specular = Kspecular * pow(saturate(RdotV), exponent);

    return (diffuse + specular);
}

float4 PSBasicTx(VSOutputShadowTx input) : SV_TARGET
{
    // Compute texture coordinates for the current point's location on the shadow map.
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (input.LightViewPosition.x / input.LightViewPosition.w * 0.5f);
    shadowTexCoords.y = 0.5f - (input.LightViewPosition.y / input.LightViewPosition.w * 0.5f);
    float pixelDepth = input.LightViewPosition.z / input.LightViewPosition.w;

    float lighting = 1;

    // Check if the pixel texture coordinate is in the view frustum of the 
    // light before doing any shadow work.
    if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
        (saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
        (pixelDepth > 0))
    {
        float NdotL = dot(input.Normal, LightDirection[0]);
        // Use an offset value to mitigate shadow artifacts due to imprecise 
        // floating-point values (shadow acne).
        //
        // This is an approximation of epsilon * tan(acos(saturate(NdotL))):
        float margin = acos(saturate(NdotL));
#ifdef LINEAR
        // The offset can be slightly smaller with smoother shadow edges.
        float epsilon = 0.0005 / margin;
#else
        float epsilon = 0.001 / margin;
#endif
        // Clamp epsilon to a fixed range so it doesn't go overboard.
        epsilon = clamp(epsilon, 0, 0.1);

        // Use the SampleCmpLevelZero Texture2D method (or SampleCmp) to sample from 
        // the shadow map, just as you would with Direct3D feature level 10_0 and
        // higher.  Feature level 9_1 only supports LessOrEqual, which returns 0 if
        // the pixel is in the shadow.
        lighting = float(shadowMap.SampleCmpLevelZero(
            shadowSampler,
            shadowTexCoords,
            pixelDepth + epsilon
        ));
        return float4(input.Diffuse * (AmbientColor.xyz + DplusS(input.Normal, LightDirection[0], NdotL, input.LightPos)), 1.f);
    }
    else
    {
        return float4(input.Diffuse * AmbientColor.xyz, 1.f);
    }
}