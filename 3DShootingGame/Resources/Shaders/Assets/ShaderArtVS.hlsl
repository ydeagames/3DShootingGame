#include "ShaderArt.hlsli"
#include "Structures.fxh"

VSOutputTx VSBasicVc(VSInputTx input)
{
	VSOutputTx output = (VSOutputTx) 0;
	output.TexCoord = input.TexCoord;
	output.PositionPS = input.Position;
	return output;
}
