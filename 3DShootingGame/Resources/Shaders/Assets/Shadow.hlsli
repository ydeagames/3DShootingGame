#include "Parameter.hlsli"
#include "Structures.fxh"
#include "Common.fxh"
#include "Lighting.fxh"

cbuffer ShadowParameters : register(b1)
{
    float4x4 SMViewProj;    // ���[���h�~�r���[�~�����ϊ��s��(�V���h�E�}�b�v�p)
};

// �e�N�X�`��
Texture2D Texture : register(t0);
Texture2D ShadowMap : register(t1); // �V���h�E�}�b�v

// �e�N�X�`���E�T���v��
SamplerState Sampler   : register(s0);
SamplerComparisonState smpCompare : register(s1);

// **************************************************
// 3D�I�u�W�F�N�g�̕`��
// **************************************************

// ���_�V�F�[�_�̊֐�
VSOutputTxEnvMap VS(VSInputNmTx vin) {
    VSOutputTxEnvMap vout;

    CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
    SetCommonVSOutputParams;

    vout.TexCoord = vin.TexCoord;

    // ���_���W�@���f�����W�n���������W�n(�V���h�E�}�b�v)
    float4 pos4 = vin.Position;
    pos4 = mul(pos4, SMViewProj);
    pos4.xyz = pos4.xyz / pos4.w;
    vout.EnvCoord.x = (pos4.x + 1.0) / 2.0;
    vout.EnvCoord.y = (-pos4.y + 1.0) / 2.0;
    vout.EnvCoord.z = pos4.z;

    // �o��
    return vout;
}

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS_NOSM(PSInputTxEnvMap pin) : SV_TARGET
{
    float4 color = Texture.Sample(Sampler, pin.TexCoord) * pin.Diffuse;

    AddSpecular(color, pin.Specular.rgb);
    ApplyFog(color, pin.Specular.w);

    return color;
}

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS(PSInputTxEnvMap pin) : SV_TARGET
{
    // �e�N�X�`���擾
    float4 texCol = PS_NOSM(pin);

	float3 shadowCoord = pin.EnvCoord;
	
	// �ő�[�x�X�΂����߂�.
	float  maxDepthSlope = max( abs( ddx( shadowCoord.z ) ), abs( ddy( shadowCoord.z ) ) );
	
	float  shadowThreshold = 1.0f;      // �V���h�E�ɂ��邩�ǂ�����臒l�ł�.
	float  bias            = 0.001f;     // �Œ�o�C�A�X�ł�.
	float  slopeScaledBias = 1.0f;     // �[�x�X��.
	float  depthBiasClamp  = 1.0f;      // �o�C�A�X�N�����v�l.
	
	float  shadowBias = bias + slopeScaledBias * maxDepthSlope;
	shadowBias = min( shadowBias, depthBiasClamp );
	
	float shadowColor     = 0.25f;
	shadowThreshold = ShadowMap.SampleCmpLevelZero(smpCompare, shadowCoord.xy, shadowCoord.z - shadowBias );
	shadowColor     = lerp( shadowColor, 1.0f, shadowThreshold );

    // �F
    return saturate(texCol * shadowColor);
}

// **************************************************
// �V���h�E�}�b�v�̕`��
// **************************************************

// ���_�V�F�[�_�̊֐�
float4 VS_SM(VSInputNmTx vin) : SV_POSITION {
    // ���_���W�@���f�����W�n���������W�n
    return mul(vin.Position, WorldViewProj);
}
