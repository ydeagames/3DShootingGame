#pragma once
// Wavefront OBJ�t�@�C���̓ǂݍ��݊֐�
//#include <d3dx11.h>
//#include <xnamath.h>
//#include <stdio.h>

//#include "xnacollision.h"
#include "StringCast.h"

// OBJ�t�@�C�����̃O���[�v���
typedef struct WFOBJ_GROUP_ {
	char name[80];  // �O���[�v��
	char mtl[80];   // �}�e���A����
	int startIndex; // �O���[�v�̊J�n�C���f�b�N�X
	int countIndex; // �O���[�v�Ɋ܂܂��C���f�b�N�X��
} WFOBJ_GROUP;

// OBJ�t�@�C���̓ǂݍ���
bool LoadWavefrontOBJ(
	FILE* fileObj, // OBJ�t�@�C��
	DirectX::XMFLOAT3* pVBufferP, // ���_�f�[�^(���W)�̃|�C���^
	int strideVBufferP, // ���_�f�[�^(���W)�̕�
	DirectX::XMFLOAT3* pVBufferN, // ���_�f�[�^(�@���x�N�g��)�̃|�C���^
	int strideVBufferN, // ���_�f�[�^(�@���x�N�g��)�̕�
	DirectX::XMFLOAT2* pVBufferT, // ���_�f�[�^(�e�N�X�`�����W)�̃|�C���^
	int strideVBufferT, // ���_�f�[�^(�e�N�X�`�����W)�̕�
	int sizeVBuffer, // ���_�o�b�t�@�ɏ������߂�f�[�^��
	int& countVBuffer, // �������񂾒��_�f�[�^��
	unsigned int* pIBuffer, // �C���f�b�N�X�E�o�b�t�@�̃|�C���^
	int sizeIBuffer, // �C���f�b�N�X�E�o�b�t�@�ɏ������߂�f�[�^��
	int& countIBuffer, // �������񂾃C���f�b�N�X��
	WFOBJ_GROUP* pGroup, // �O���[�v���o�b�t�@
	int sizeGroup, // �O���[�u���o�b�t�@�̃T�C�Y
	int& countGroup, // �������񂾃O���[�v���̐�
	char* pMtlFileName, // �}�e���A���E�t�@�C�������󂯎��o�b�t�@
	int sizeMtlFileName) // �o�b�t�@�̑傫��
;

// �}�e���A���E�f�[�^
typedef struct WFOBJ_MTL_ {
	char name[80];     // �}�e���A����
	float Kd[3];       // �f�B�t���[�Y�F�B�l�́u0�v�`�u1�v�B
	float Ks[3];       // �X�y�L�����F�B�l�́u0�v�`�u1�v�B
	float Ka[3];       // �A���r�G���g(��)�F�B�l�́u0�v�`�u1�v�B
	float d;           // �����x�B�l�́u0�v(����)�`�u1�v(�s����)�B
	float Ns;          // ����B�l�́A�u0�v�`�B 
	float Ni;          // ���ܗ��B�l�́u1�v�ȏ�B�u1�v�͋��܂Ȃ��B
	char map_Kd[80];   // �f�B�t���[�Y�E�}�b�v�B��ʓI�ȃe�N�X�`���B
	char map_Ks[80];   // �X�y�L�����E�}�b�v�B
	char map_Ka[80];   // ���}�b�v�B
	char map_Bump[80]; // �o���v�E�}�b�v�B
	char map_D[80];    // �����}�b�v�B
	char refl[80];     // ���˃}�b�v�B
} WFOBJ_MTL;

// MTL�t�@�C���̓ǂݍ���
bool LoadWavefrontMTL(
	FILE* fileMtl, // MTL�t�@�C��
	WFOBJ_MTL* pMtl, // �}�e���A���f�[�^���󂯎��z��
	int sizeMtl, // �z��T�C�Y
	int& countMtl) // �ǂݍ��񂾃}�e���A���̐�
;

// �w���p�[�N���X
class CWavefrontObj {
	ID3D11Device*        m_pD3DDevice;
	ID3D11DeviceContext* m_pD3DDeviceContext;

	ID3D11Buffer* m_pVerBuffer;		// ���_�o�b�t�@�̃C���^�[�t�F�C�X
	ID3D11Buffer* m_pIdxBuffer;		// �C���f�b�N�X�E�o�b�t�@�̃C���^�[�t�F�C�X
	ID3D11Buffer* m_pIdxAdjBuffer;  // �C���f�b�N�X�E�o�b�t�@(�אڕt��)�̃C���^�[�t�F�C�X

	WFOBJ_GROUP m_Group[256];   // �O���[�v(�ő�256)
	int m_countGroup;           // �O���[�v��
	int m_countVBuffer;
	int m_countIBuffer;

	//XNA::Sphere m_spherebox; //���E���̏��
	bool     m_bCrack;       // �����
public:
	CWavefrontObj() : m_pD3DDevice(NULL), m_pD3DDeviceContext(NULL), m_pVerBuffer(NULL), m_pIdxBuffer(NULL), m_countGroup(0), m_countVBuffer(0), m_countIBuffer(0), m_bCrack(true) {}
	~CWavefrontObj() { Cleanup(); }

	// �f�[�^
	ID3D11Buffer* GetVerBuffer() { return m_pVerBuffer; }
	ID3D11Buffer* GetIdxBuffer() { return m_pIdxBuffer; }
	WFOBJ_GROUP* GetGroup()      { return m_Group; }

	// �f�[�^��
	int GetCountVBuffer()        { return m_countVBuffer; }
	int GetCountIBuffer()        { return m_countIBuffer; }
	int GetCountGroup()          { return m_countGroup; }

	// ���E��
	//DirectX::XMFLOAT3 GetBoundingSphereCenter() { return m_spherebox.Center; }
	//FLOAT    GetBoundingSphereRadius() { return m_spherebox.Radius; }
	bool     IsCrack() { return m_bCrack; }

	// OBJ�t�@�C����ǂݍ��݁A���_/�C���f�b�N�X�E�o�b�t�@���쐬����
	// pD3DDevice      : ���_/�C���f�b�N�X�E�o�b�t�@���쐬����Direct3D11�f�o�C�X
	// objfilename     : OBJ�t�@�C����
	// mtlfilename     : OBJ�t�@�C���̃}�e���A�����i�[���Ă���MTL�t�@�C�������󂯎��z��
	// sizemtlfilename : mtlfilename�z��̃T�C�Y
	bool Load(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, char* objfilename, char* mtlfilename,
	          int sizemtlfilename);

	// IA�ɒ��_�o�b�t�@�ƃC���f�b�N�X�E�o�b�t�@��ݒ�
	void SetIA(bool bAdj = false);

	// �`��
	// nameGroup : �`�悷��O���[�v��
	// numGroup  : �`�悷��O���[�v�ԍ�
	// bAdj      : �אڕt���f�[�^��`��
	void Draw(char* nameGroup, bool bAdj = false);

	void Draw(int numGroup, bool bAdj = false);
	// �C���X�^���X�`��
	// numInst   : �`�悷��C���X�^���X��
	// nameGroup : �`�悷��O���[�v��
	// numGroup  : �`�悷��O���[�v�ԍ�
	// bAdj      : �אڕt���f�[�^��`��
	void DrawInstanced(int numInst, char* nameGroup, bool bAdj = false);

	void DrawInstanced(int numInst, int numGroup, bool bAdj = false);

	// �I������or������
	void Cleanup();
};

class CWavefrontMtl {
	char m_MaterialFileName[80];

	char m_texFileName[256][80];
	ID3D11ShaderResourceView* m_Texture[256];
	int m_countTexture;

	WFOBJ_MTL m_Material[256];
	int m_countMaterial;

	ID3D11ShaderResourceView* m_DefaultTexture;

public:
	CWavefrontMtl() : m_countTexture(0), m_countMaterial(0), m_DefaultTexture(NULL)
	{ m_MaterialFileName[0] = NULL; }
	~CWavefrontMtl() { Cleanup(); }

	// �}�e���A���E�t�@�C����
	char* GetMaterialFileName() { return m_MaterialFileName; }

	// �e�N�X�`���̃V�F�[�_�E���\�[�X�E�r���[(�ǂݍ���ł��Ȃ��ꍇ�́ANULL��Ԃ�)
	// texName : �e�N�X�`���E�t�@�C����
	ID3D11ShaderResourceView* GetTexture(char* texName);

	// �e�N�X�`���̃V�F�[�_�E���\�[�X�E�r���[(�ǂݍ���ł��Ȃ��ꍇ�́Am_DefaultTexture��Ԃ�)
	// texName : �e�N�X�`���E�t�@�C����
	ID3D11ShaderResourceView* GetTextureDefault(char* texName);

	// �e�N�X�`���̃V�F�[�_�E���\�[�X�E�r���[(�ǂݍ���ł��Ȃ��ꍇ�́A�V���ɓǂݍ���)
	// texName : �e�N�X�`���E�t�@�C����
	ID3D11ShaderResourceView* GetTexture(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext,
	                                     char* texName);

	// �}�e���A�����̎擾
	// mtlName : �}�e���A����
	WFOBJ_MTL* GetMaterial(char* mtlName);

	// MTL�t�@�C���̓ǂݍ���
	// pD3DDevice  : �e�N�X�`���̃V�F�[�_�E���\�[�X�E�r���[���쐬����Direct3D11�f�o�C�X
	// mtlfilename : MTL�t�@�C����
	// deffilename : �f�t�H���g�̃e�N�X�`���E�t�@�C����
	// defcolor    : �f�t�H���g�̃}�e���A���F
	bool Load(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, char* mtlfilename, char* deffilename);

	// �I��������������
	void Cleanup();
};