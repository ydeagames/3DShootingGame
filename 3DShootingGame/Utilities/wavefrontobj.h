#pragma once
// Wavefront OBJファイルの読み込み関数
//#include <d3dx11.h>
//#include <xnamath.h>
//#include <stdio.h>

//#include "xnacollision.h"
#include "StringCast.h"

// OBJファイル内のグループ情報
typedef struct WFOBJ_GROUP_ {
	char name[80];  // グループ名
	char mtl[80];   // マテリアル名
	int startIndex; // グループの開始インデックス
	int countIndex; // グループに含まれるインデックス数
} WFOBJ_GROUP;

// OBJファイルの読み込み
bool LoadWavefrontOBJ(
	FILE* fileObj, // OBJファイル
	DirectX::XMFLOAT3* pVBufferP, // 頂点データ(座標)のポインタ
	int strideVBufferP, // 頂点データ(座標)の幅
	DirectX::XMFLOAT3* pVBufferN, // 頂点データ(法線ベクトル)のポインタ
	int strideVBufferN, // 頂点データ(法線ベクトル)の幅
	DirectX::XMFLOAT2* pVBufferT, // 頂点データ(テクスチャ座標)のポインタ
	int strideVBufferT, // 頂点データ(テクスチャ座標)の幅
	int sizeVBuffer, // 頂点バッファに書き込めるデータ数
	int& countVBuffer, // 書き込んだ頂点データ数
	unsigned int* pIBuffer, // インデックス・バッファのポインタ
	int sizeIBuffer, // インデックス・バッファに書き込めるデータ数
	int& countIBuffer, // 書き込んだインデックス数
	WFOBJ_GROUP* pGroup, // グループ情報バッファ
	int sizeGroup, // グルーブ情報バッファのサイズ
	int& countGroup, // 書き込んだグループ情報の数
	char* pMtlFileName, // マテリアル・ファイル名を受け取るバッファ
	int sizeMtlFileName) // バッファの大きさ
;

// マテリアル・データ
typedef struct WFOBJ_MTL_ {
	char name[80];     // マテリアル名
	float Kd[3];       // ディフューズ色。値は「0」～「1」。
	float Ks[3];       // スペキュラ色。値は「0」～「1」。
	float Ka[3];       // アンビエント(環境)色。値は「0」～「1」。
	float d;           // 透明度。値は「0」(透明)～「1」(不透明)。
	float Ns;          // 光沢。値は、「0」～。 
	float Ni;          // 屈折率。値は「1」以上。「1」は屈折なし。
	char map_Kd[80];   // ディフューズ・マップ。一般的なテクスチャ。
	char map_Ks[80];   // スペキュラ・マップ。
	char map_Ka[80];   // 環境マップ。
	char map_Bump[80]; // バンプ・マップ。
	char map_D[80];    // 透明マップ。
	char refl[80];     // 反射マップ。
} WFOBJ_MTL;

// MTLファイルの読み込み
bool LoadWavefrontMTL(
	FILE* fileMtl, // MTLファイル
	WFOBJ_MTL* pMtl, // マテリアルデータを受け取る配列
	int sizeMtl, // 配列サイズ
	int& countMtl) // 読み込んだマテリアルの数
;

// ヘルパークラス
class CWavefrontObj {
	ID3D11Device*        m_pD3DDevice;
	ID3D11DeviceContext* m_pD3DDeviceContext;

	ID3D11Buffer* m_pVerBuffer;		// 頂点バッファのインターフェイス
	ID3D11Buffer* m_pIdxBuffer;		// インデックス・バッファのインターフェイス
	ID3D11Buffer* m_pIdxAdjBuffer;  // インデックス・バッファ(隣接付き)のインターフェイス

	WFOBJ_GROUP m_Group[256];   // グループ(最大256)
	int m_countGroup;           // グループ数
	int m_countVBuffer;
	int m_countIBuffer;

	//XNA::Sphere m_spherebox; //境界球の情報
	bool     m_bCrack;       // 割れ目
public:
	CWavefrontObj() : m_pD3DDevice(NULL), m_pD3DDeviceContext(NULL), m_pVerBuffer(NULL), m_pIdxBuffer(NULL), m_countGroup(0), m_countVBuffer(0), m_countIBuffer(0), m_bCrack(true) {}
	~CWavefrontObj() { Cleanup(); }

	// データ
	ID3D11Buffer* GetVerBuffer() { return m_pVerBuffer; }
	ID3D11Buffer* GetIdxBuffer() { return m_pIdxBuffer; }
	WFOBJ_GROUP* GetGroup()      { return m_Group; }

	// データ数
	int GetCountVBuffer()        { return m_countVBuffer; }
	int GetCountIBuffer()        { return m_countIBuffer; }
	int GetCountGroup()          { return m_countGroup; }

	// 境界球
	//DirectX::XMFLOAT3 GetBoundingSphereCenter() { return m_spherebox.Center; }
	//FLOAT    GetBoundingSphereRadius() { return m_spherebox.Radius; }
	bool     IsCrack() { return m_bCrack; }

	// OBJファイルを読み込み、頂点/インデックス・バッファを作成する
	// pD3DDevice      : 頂点/インデックス・バッファを作成するDirect3D11デバイス
	// objfilename     : OBJファイル名
	// mtlfilename     : OBJファイルのマテリアルを格納しているMTLファイル名を受け取る配列
	// sizemtlfilename : mtlfilename配列のサイズ
	bool Load(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, char* objfilename, char* mtlfilename,
	          int sizemtlfilename);

	// IAに頂点バッファとインデックス・バッファを設定
	void SetIA(bool bAdj = false);

	// 描画
	// nameGroup : 描画するグループ名
	// numGroup  : 描画するグループ番号
	// bAdj      : 隣接付きデータを描画
	void Draw(char* nameGroup, bool bAdj = false);

	void Draw(int numGroup, bool bAdj = false);
	// インスタンス描画
	// numInst   : 描画するインスタンス数
	// nameGroup : 描画するグループ名
	// numGroup  : 描画するグループ番号
	// bAdj      : 隣接付きデータを描画
	void DrawInstanced(int numInst, char* nameGroup, bool bAdj = false);

	void DrawInstanced(int numInst, int numGroup, bool bAdj = false);

	// 終了処理or初期化
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

	// マテリアル・ファイル名
	char* GetMaterialFileName() { return m_MaterialFileName; }

	// テクスチャのシェーダ・リソース・ビュー(読み込んでいない場合は、NULLを返す)
	// texName : テクスチャ・ファイル名
	ID3D11ShaderResourceView* GetTexture(char* texName);

	// テクスチャのシェーダ・リソース・ビュー(読み込んでいない場合は、m_DefaultTextureを返す)
	// texName : テクスチャ・ファイル名
	ID3D11ShaderResourceView* GetTextureDefault(char* texName);

	// テクスチャのシェーダ・リソース・ビュー(読み込んでいない場合は、新たに読み込む)
	// texName : テクスチャ・ファイル名
	ID3D11ShaderResourceView* GetTexture(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext,
	                                     char* texName);

	// マテリアル情報の取得
	// mtlName : マテリアル名
	WFOBJ_MTL* GetMaterial(char* mtlName);

	// MTLファイルの読み込み
	// pD3DDevice  : テクスチャのシェーダ・リソース・ビューを作成するDirect3D11デバイス
	// mtlfilename : MTLファイル名
	// deffilename : デフォルトのテクスチャ・ファイル名
	// defcolor    : デフォルトのマテリアル色
	bool Load(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, char* mtlfilename, char* deffilename);

	// 終了処理＆初期化
	void Cleanup();
};
