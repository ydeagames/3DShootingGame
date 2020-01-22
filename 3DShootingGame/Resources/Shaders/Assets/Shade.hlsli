struct VSOutputShadowTx
{
    float4 Diffuse    : COLOR0;
    float4 Specular   : COLOR1;
    float2 TexCoord   : TEXCOORD0;
    float4 PositionPS : SV_Position;
    float3 Normal     : NORMAL;
    float4 lightSpacePos :    SHADOW;
    float3 view       : POSITION;
};
