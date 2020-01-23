struct PixelInputType
{
    float4 Diffuse    : COLOR0;
    float4 Specular   : COLOR1;
    float4 PositionPS : SV_Position;
    float4 DepthPosition : TEXCOORD0;
};
