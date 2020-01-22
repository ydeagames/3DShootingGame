#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"

VSOutputTx VSBasicVc(VSInputTx vin)
{
    VSOutputTx vout;

    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    return vout;
}
