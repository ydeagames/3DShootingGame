#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"
#include "Shadow.hlsli"

PixelInputType DepthVertexShader(VSInput vin)
{
    PixelInputType vout;

    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParams;

    // Store the position value in a second input value for depth value calculations.
    vout.DepthPosition = vout.PositionPS;

    return vout;
}
