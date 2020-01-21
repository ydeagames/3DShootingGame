#include "Parameter.hlsli"
#include "Structures.fxh"

float4 PSBasicTx(PSInputTx input) : SV_TARGET
{
	discard;
	return float4(0, 0, 0, 0);
}