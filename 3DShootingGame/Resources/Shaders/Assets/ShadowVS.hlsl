#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"

VSOutput VSBasic(VSInput vin)
{
    VSOutput vout;

    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParams;

    return vout;
}
