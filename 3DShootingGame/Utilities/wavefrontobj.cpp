#include "pch.h"
#include "wavefrontobj.h"

bool LoadWavefrontOBJ(FILE* fileObj, DirectX::XMFLOAT3* pVBufferP, int strideVBufferP, DirectX::XMFLOAT3* pVBufferN,
                      int strideVBufferN, DirectX::XMFLOAT2* pVBufferT, int strideVBufferT, int sizeVBuffer,
                      int& countVBuffer,
                      unsigned* pIBuffer, int sizeIBuffer, int& countIBuffer, WFOBJ_GROUP* pGroup, int sizeGroup,
                      int& countGroup,
                      char* pMtlFileName, int sizeMtlFileName)
{
	countVBuffer = countIBuffer = 0;
	countGroup = 1;
	pGroup[0].name[0] = pGroup[0].mtl[0] = NULL;
	pGroup[0].startIndex = pGroup[0].countIndex = 0;
	pMtlFileName[0] = NULL;

	static DirectX::XMFLOAT3 V[0xffff + 1];
	static DirectX::XMFLOAT2 VT[0xffff + 1];
	static DirectX::XMFLOAT3 VN[0xffff + 1];
	static unsigned int IDX[0xffff + 1][3];
	int countV = 0, countVT = 0, countVN = 0, countIDX = 0;

	// OBJ��ǂݍ���
	while (TRUE)
	{
		// 1�s�P�ʂœǂݍ���
		static char buffer[1024], op[4][256];
		char* pp = fgets(buffer, sizeof(buffer), fileObj);
		if (pp == NULL)
			break;

		int n;
		// ���W
		n = sscanf_s(buffer, "v %f %f %f", &V[countV].x, &V[countV].y, &V[countV].z);
		if (n > 0)
		{
			++countV;
			if (countV > 0xffff) return false;
			continue;
		}
		// �e�N�X�`�����W
		n = sscanf_s(buffer, "vt %f %f", &VT[countVT].x, &VT[countVT].y);
		if (n > 0)
		{
			++countVT;
			if (countVT > 0xffff) return false;
			continue;
		}
		// �@���x�N�g��
		n = sscanf_s(buffer, "vn %f %f %f",
		             &VN[countVN].x, &VN[countVN].y, &VN[countVN].z);
		if (n > 0)
		{
			++countVN;
			if (countVN > 0xffff) return false;
			continue;
		}
		// �O���[�v��
		n = sscanf_s(buffer, "g %s",
		             pGroup[countGroup].name, sizeof(pGroup[countGroup].name));
		if (n > 0)
		{
			pGroup[countGroup].mtl[0] = NULL;
			pGroup[countGroup].startIndex = countIDX;
			pGroup[countGroup].countIndex = 0;
			++countGroup;
			if (countGroup >= sizeGroup)
				return false;
			continue;
		}
		// �}�e���A����
		n = sscanf_s(buffer, "usemtl %s",
		             pGroup[countGroup - 1].mtl, sizeof(pGroup[countGroup - 1].mtl));
		if (n > 0)
			continue;
		// �}�e���A���E�t�@�C����
		n = sscanf_s(buffer, "mtllib %s", pMtlFileName, sizeMtlFileName);
		if (n > 0)
			continue;
		// ��(�O�p�`�܂��͎l�p�`�Ɖ���)
		n = sscanf_s(buffer, "f %s %s %s %s",
		             op[0], sizeof(op[0]), op[1], sizeof(op[1]),
		             op[2], sizeof(op[2]), op[3], sizeof(op[3]));
		if (n == 0)
			continue;
		if (n < 3)
			return false;
		int f[4][3];
		for (int i = 0; i < n; ++i)
		{
			int iv = 0, it = 0, in = 0;
			if (sscanf_s(op[i], "%i/%i/%i", &iv, &it, &in) == 1)
				sscanf_s(op[i], "%i//%i", &iv, &in);
			f[i][0] = (iv < 0) ? countV - iv + 1 : iv;
			f[i][1] = (it < 0) ? countVT - it + 1 : it;
			f[i][2] = (in < 0) ? countVN - in + 1 : in;
		}
		for (int count = 0; count < n - 2; ++count)
		{
			if (countIDX + 3 > 0xffff)
				return false;
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&V[f[0][0] - 1]),
			                                                  DirectX::XMLoadFloat3(&V[f[count + 1][0] - 1]));
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(vec)) < 0.00001f)
				continue;
			vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&V[f[0][0] - 1]),
			                                DirectX::XMLoadFloat3(&V[f[count + 2][0] - 1]));
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(vec)) < 0.00001f)
				continue;
			vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&V[f[count + 1][0] - 1]),
			                                DirectX::XMLoadFloat3(&V[f[count + 2][0] - 1]));
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(vec)) < 0.00001f)
				continue;
			for (int h = 0; h < 3; ++h)
			{
				IDX[countIDX][h] = f[0][h];
				IDX[countIDX + 1][h] = f[count + 1][h];
				IDX[countIDX + 2][h] = f[count + 2][h];
			}
			countIDX += 3;
			pGroup[countGroup - 1].countIndex += 3;
		}
	};

	// �ߐڒ��_����������
	for (int i = 0; i < countV; ++i)
	{
		for (int h = i + 1; h < countV; ++h)
		{
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&V[i]),
			                                                  DirectX::XMLoadFloat3(&V[h]));
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(vec)) >= 0.0001f)
				continue;
			for (int j = 0; j < countIDX; ++j)
			{
				if (IDX[j][0] == h + 1)
					IDX[j][0] = i + 1;
			}
		}
	}

	// �C���f�b�N�X�E�o�b�t�@��g�ݗ��Ă�
	static int idx[0xffff][3];
	int num;
	countVBuffer = 0;
	for (int i = 0; i < countIDX; ++i)
	{
		for (num = 0; num < countVBuffer; ++num)
			if (idx[num][0] == IDX[i][0] && idx[num][1] == IDX[i][1]
				&& idx[num][2] == IDX[i][2])
				break;
		if (num >= sizeVBuffer)
			return false;
		if (num >= countVBuffer)
		{
			idx[num][0] = IDX[i][0];
			idx[num][1] = IDX[i][1];
			idx[num][2] = IDX[i][2];
			++countVBuffer;
		}
		pIBuffer[countIBuffer] = num;
		++countIBuffer;
		if (countIBuffer >= sizeIBuffer)
			return false;
	}

	// ���_�o�b�t�@��g�ݗ��Ă�
	for (int i = 0; i < countVBuffer; ++i)
	{
		--idx[i][0];
		--idx[i][1];
		--idx[i][2];
	}
	DirectX::XMFLOAT3* pP = pVBufferP;
	DirectX::XMFLOAT2* pT = pVBufferT;
	DirectX::XMFLOAT3* pN = pVBufferN;
	for (int i = 0; i < countVBuffer; ++i)
	{
		// ���W
		if ((idx[i][0] < 0) || (idx[i][0] >= countV))
			pP->x = pP->y = pP->z = 0.0f;
		else *pP = V[idx[i][0]];
		pP = (DirectX::XMFLOAT3*)(((char*)pP) + strideVBufferP);
		// �e�N�X�`�����W
		if ((idx[i][1] < 0) || (idx[i][1] >= countVT))
			pT->x = pT->y = 0.0f;
		else *pT = VT[idx[i][1]];
		pT = (DirectX::XMFLOAT2*)(((char*)pT) + strideVBufferT);
		// �@���x�N�g��
		if ((idx[i][2] < 0) || (idx[i][2] >= countVN))
			pN->x = pN->y = pN->z = 1.0f;
		else *pN = VN[idx[i][2]];
		pN = (DirectX::XMFLOAT3*)(((char*)pN) + strideVBufferN);
	}

	return true;
}

bool LoadWavefrontMTL(FILE* fileMtl, WFOBJ_MTL* pMtl, int sizeMtl, int& countMtl)
{
	countMtl = -1;

	// MTL��ǂݍ���
	static char buffer[1024];
	while (TRUE)
	{
		// 1�s�P�ʂœǂݍ���
		char* pp = fgets(buffer, sizeof(buffer), fileMtl);
		if (pp == NULL)
			break;

		int n;
		// �}�e���A����
		n = sscanf_s(buffer, "newmtl %s",
		             pMtl[countMtl + 1].name, sizeof(pMtl[countMtl + 1].name));
		if (n > 0)
		{
			++countMtl;
			if (countMtl >= sizeMtl)
				return false;
			for (int i = 0; i < 3; ++i)
			{
				pMtl[countMtl].Kd[i] = 1.0f;
				pMtl[countMtl].Ks[i] = 1.0f;
				pMtl[countMtl].Ka[i] = 1.0f;
			}
			pMtl[countMtl].d = 1.0f;
			pMtl[countMtl].Ns = 0.0f;
			pMtl[countMtl].Ni = 1.0f;
			pMtl[countMtl].map_Kd[0] = NULL;
			pMtl[countMtl].map_Ks[0] = NULL;
			pMtl[countMtl].map_Ka[0] = NULL;
			pMtl[countMtl].map_Bump[0] = NULL;
			pMtl[countMtl].map_D[0] = NULL;
			pMtl[countMtl].refl[0] = NULL;
			continue;
		}
		if (countMtl < 0)
			continue;
		// �f�B�t���[�Y�F
		n = sscanf_s(buffer, "Kd %f %f %f", &pMtl[countMtl].Kd[0],
		             &pMtl[countMtl].Kd[1], &pMtl[countMtl].Kd[2]);
		if (n > 0) continue;
		// �X�y�L�����F
		n = sscanf_s(buffer, "Ks %f %f %f", &pMtl[countMtl].Ks[0],
		             &pMtl[countMtl].Ks[1], &pMtl[countMtl].Ks[2]);
		if (n > 0) continue;
		// �A���r�G���g�F
		n = sscanf_s(buffer, "Ka %f %f %f", &pMtl[countMtl].Ka[0],
		             &pMtl[countMtl].Ka[1], &pMtl[countMtl].Ka[2]);
		if (n > 0) continue;
		// �����x
		n = sscanf_s(buffer, "d %f", &pMtl[countMtl].d);
		if (n > 0) continue;
		// ����
		n = sscanf_s(buffer, "Ns %f", &pMtl[countMtl].Ns);
		if (n > 0) continue;
		// ���ܗ�
		n = sscanf_s(buffer, "Ni %f", &pMtl[countMtl].Ni);
		if (n > 0) continue;
		// �f�B�t���[�Y�E�}�b�v
		n = sscanf_s(buffer, "map_Kd %s",
		             pMtl[countMtl].map_Kd, sizeof(pMtl[countMtl].map_Kd));
		if (n > 0) continue;
		// �o���v�E�}�b�v
		n = sscanf_s(buffer, "map_Bump %s",
		             pMtl[countMtl].map_Bump, sizeof(pMtl[countMtl].map_Bump));
		if (n > 0) continue;
		// �X�y�L�����E�}�b�v
		n = sscanf_s(buffer, "map_Ks %s",
		             pMtl[countMtl].map_Ks, sizeof(pMtl[countMtl].map_Ks));
		if (n > 0) continue;
		// ���}�b�v
		n = sscanf_s(buffer, "map_Ka %s",
		             pMtl[countMtl].map_Ka, sizeof(pMtl[countMtl].map_Ka));
		if (n > 0) continue;
	};
	++countMtl;

	return true;
}

bool CWavefrontObj::Load(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, char* objfilename,
                         char* mtlfilename, int sizemtlfilename)
{
	Cleanup();
	m_pD3DDevice = pD3DDevice;
	m_pD3DDeviceContext = pD3DDeviceContext;

	// **********************************************************
	// OBJ�t�@�C���̓ǂݍ���
	FILE* fileObj;
	if (fopen_s(&fileObj, objfilename, "rt") != 0)
		return false;

	struct verstr
	{
		DirectX::XMFLOAT3 v;
		DirectX::XMFLOAT3 n;
		DirectX::XMFLOAT2 t;
	}* pVer = new verstr[0xffff];
	unsigned int* pI = new unsigned int[0xffff];
	unsigned int* pIAdj = new unsigned int[0xffff];

	bool ret = LoadWavefrontOBJ(fileObj,
	                            &pVer[0].v, sizeof(verstr), &pVer[0].n, sizeof(verstr), &pVer[0].t, sizeof(verstr),
	                            0xffff, m_countVBuffer,
	                            pI, 0xffff, m_countIBuffer,
	                            m_Group, _countof(m_Group), m_countGroup,
	                            mtlfilename, sizemtlfilename);

	fclose(fileObj);

	if ((ret == false) || (m_countVBuffer == 0) || (m_countIBuffer == 0))
	{
		delete[] pVer;
		delete[] pI;
		delete[] pIAdj;
		Cleanup();
		return false;
	}

	// **********************************************************
	// �אڏ��̌v�Z
	int countAdj = m_countIBuffer / 3; // �v���~�e�B�u��
	for (int ic = 0; ic < m_countIBuffer; ic += 3)
	{
		// �O�p�`���X�g�̃C���f�b�N�X
		int iI[3] = {pI[ic], pI[ic + 1], pI[ic + 2]};
		DirectX::XMFLOAT3 v[3] = {pVer[iI[0]].v, pVer[iI[1]].v, pVer[iI[2]].v};

		// �אڕt���O�p�`���X�g�̃C���f�b�N�X
		int icadj = ic * 2;
		pIAdj[icadj] = iI[0];
		pIAdj[icadj + 1] = (UINT)-1;
		pIAdj[icadj + 2] = iI[1];
		pIAdj[icadj + 3] = (UINT)-1;
		pIAdj[icadj + 4] = iI[2];
		pIAdj[icadj + 5] = (UINT)-1;

		// �אڏ��̌���
		for (int is = 0; is < m_countIBuffer; is += 3)
		{
			if (ic == is)
				continue;
			int iIs[3] = {pI[is], pI[is + 1], pI[is + 2]};
			DirectX::XMFLOAT3 s[3] = {pVer[iIs[0]].v, pVer[iIs[1]].v, pVer[iIs[2]].v};

			for (int i1 = 0; i1 < 3; ++i1)
			{
				int i2 = (i1 + 1) % 3, i3 = (i1 + 2) % 3, ia = icadj + i1 * 2 + 1;
				if (pIAdj[ia] != (UINT)-1)
					continue;
				for (int h1 = 0; h1 < 3; ++h1)
				{
					int h2 = (h1 + 1) % 3, h3 = (h1 + 2) % 3;
					BOOL flag1 = DirectX::XMVector3Equal(XMLoadFloat3(&v[i1]), XMLoadFloat3(&s[h2]));
					BOOL flag2 = DirectX::XMVector3Equal(XMLoadFloat3(&v[i2]), XMLoadFloat3(&s[h1]));
					if (flag1 && flag2)
					{
						pIAdj[ia] = iIs[h3];
						break;
					}
				}
			}

			if ((pIAdj[icadj + 1] != (UINT)-1) &&
				(pIAdj[icadj + 3] != (UINT)-1) &&
				(pIAdj[icadj + 5] != (UINT)-1))
			{
				--countAdj;
				break;
			}
		}
	}
	m_bCrack = (countAdj > 0); // ����ڂ���

	// **********************************************************
	// ���E���̌v�Z
	//DirectX::ComputeBoundingSphereFromPoints(&m_spherebox, m_countVBuffer, &pVer[0].v, sizeof(verstr));

	// **********************************************************
	// ���_�o�b�t�@�̒�`
	D3D11_BUFFER_DESC verBufferDesc;
	verBufferDesc.Usage = D3D11_USAGE_DEFAULT; // �f�t�H���g�g�p�@
	verBufferDesc.ByteWidth = (sizeof(DirectX::XMFLOAT3) * 2 + sizeof(DirectX::XMFLOAT2)) * m_countVBuffer;
	verBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@
	verBufferDesc.CPUAccessFlags = 0;
	verBufferDesc.MiscFlags = 0;
	verBufferDesc.StructureByteStride = 0;

	// ���_�o�b�t�@�̃T�u���\�[�X�̒�`
	D3D11_SUBRESOURCE_DATA verSubData;
	verSubData.pSysMem = pVer; // �o�b�t�@�E�f�[�^�̏����l
	verSubData.SysMemPitch = 0;
	verSubData.SysMemSlicePitch = 0;

	// ���_�o�b�t�@�̍쐬
	HRESULT hr = m_pD3DDevice->CreateBuffer(&verBufferDesc, &verSubData, &m_pVerBuffer);
	delete[] pVer;
	if (FAILED(hr))
	{
		delete[] pI;
		delete[] pIAdj;
		Cleanup();
		return false;
	}

	// **********************************************************
	// �C���f�b�N�X�E�o�b�t�@�̒�`
	D3D11_BUFFER_DESC idxBufferDesc;
	idxBufferDesc.Usage = D3D11_USAGE_DEFAULT; // �f�t�H���g�g�p�@
	idxBufferDesc.ByteWidth = sizeof(UINT) * m_countIBuffer;
	idxBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // �C���f�b�N�X�E�o�b�t�@
	idxBufferDesc.CPUAccessFlags = 0;
	idxBufferDesc.MiscFlags = 0;
	idxBufferDesc.StructureByteStride = 0;

	// �C���f�b�N�X�E�o�b�t�@�̃T�u���\�[�X�̒�`
	D3D11_SUBRESOURCE_DATA idxSubData;
	idxSubData.pSysMem = pI; // �o�b�t�@�E�f�[�^�̏����l
	idxSubData.SysMemPitch = 0;
	idxSubData.SysMemSlicePitch = 0;

	// �C���f�b�N�X�E�o�b�t�@�̍쐬
	hr = m_pD3DDevice->CreateBuffer(&idxBufferDesc, &idxSubData, &m_pIdxBuffer);
	delete[] pI;
	if (FAILED(hr))
	{
		delete[] pIAdj;
		Cleanup();
		return false;
	}

	// **********************************************************
	// �אڕt���C���f�b�N�X�E�o�b�t�@�̒�`
	idxBufferDesc.ByteWidth *= 2;
	idxSubData.pSysMem = pIAdj;

	// �C���f�b�N�X�E�o�b�t�@�̍쐬
	hr = m_pD3DDevice->CreateBuffer(&idxBufferDesc, &idxSubData, &m_pIdxAdjBuffer);
	delete[] pIAdj;
	if (FAILED(hr))
	{
		Cleanup();
		return false;
	}

	return true;
}

void CWavefrontObj::SetIA(bool bAdj)
{
	// IA�ɒ��_�o�b�t�@��ݒ�
	UINT strides[1] = {sizeof(DirectX::XMFLOAT3) * 2 + sizeof(DirectX::XMFLOAT2)};
	UINT offsets[1] = {0};
	m_pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVerBuffer, strides, offsets);
	// IA�ɃC���f�b�N�X�E�o�b�t�@��ݒ�
	m_pD3DDeviceContext->IASetIndexBuffer(bAdj ? m_pIdxAdjBuffer : m_pIdxBuffer, DXGI_FORMAT_R32_UINT, 0);
	// IA�Ƀv���~�e�B�u�̎�ނ�ݒ�
	m_pD3DDeviceContext->IASetPrimitiveTopology(bAdj
		                                            ? D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ
		                                            : D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CWavefrontObj::Draw(char* nameGroup, bool bAdj)
{
	if (nameGroup == NULL)
		return;
	for (int i = 0; i < m_countGroup; ++i)
		if (strcmp(m_Group[i].name, nameGroup) == 0)
			Draw(i, bAdj);
}

void CWavefrontObj::Draw(int numGroup, bool bAdj)
{
	if ((numGroup < 0) || (numGroup >= m_countGroup))
		return;
	if (bAdj)
		m_pD3DDeviceContext->DrawIndexed(m_Group[numGroup].countIndex * 2, m_Group[numGroup].startIndex * 2, 0);
	else
		m_pD3DDeviceContext->DrawIndexed(m_Group[numGroup].countIndex, m_Group[numGroup].startIndex, 0);
}

void CWavefrontObj::DrawInstanced(int numInst, char* nameGroup, bool bAdj)
{
	if (nameGroup == NULL)
		return;
	for (int i = 0; i < m_countGroup; ++i)
		if (strcmp(m_Group[i].name, nameGroup) == 0)
			DrawInstanced(numInst, i, bAdj);
}

void CWavefrontObj::DrawInstanced(int numInst, int numGroup, bool bAdj)
{
	if ((numGroup < 0) || (numGroup >= m_countGroup))
		return;
	if (bAdj)
		m_pD3DDeviceContext->DrawIndexedInstanced(m_Group[numGroup].countIndex * 2, numInst,
		                                          m_Group[numGroup].startIndex * 2, 0, 0);
	else
		m_pD3DDeviceContext->DrawIndexedInstanced(m_Group[numGroup].countIndex, numInst, m_Group[numGroup].startIndex,
		                                          0, 0);
}

void CWavefrontObj::Cleanup()
{
	m_pD3DDevice = NULL;
	m_pD3DDeviceContext = NULL;
	if (m_pVerBuffer != NULL)
	{
		m_pVerBuffer->Release();
		m_pVerBuffer = NULL;
	}
	if (m_pIdxBuffer != NULL)
	{
		m_pIdxBuffer->Release();
		m_pIdxBuffer = NULL;
	}
	if (m_pIdxAdjBuffer != NULL)
	{
		m_pIdxAdjBuffer->Release();
		m_pIdxAdjBuffer = NULL;
	}
	m_countGroup = 0;
	m_countVBuffer = 0;
	m_countIBuffer = 0;
}

ID3D11ShaderResourceView* CWavefrontMtl::GetTexture(char* texName)
{
	if (texName == NULL)
		return NULL;
	for (int i = 0; i < m_countTexture; ++i)
	{
		if (strcmp(texName, m_texFileName[i]) == 0)
			return m_Texture[i];
	}
	return NULL;
}

ID3D11ShaderResourceView* CWavefrontMtl::GetTextureDefault(char* texName)
{
	if (texName == NULL)
		return m_DefaultTexture;
	for (int i = 0; i < m_countTexture; ++i)
	{
		if (strcmp(texName, m_texFileName[i]) == 0)
			return m_Texture[i];
	}
	return m_DefaultTexture;
}

ID3D11ShaderResourceView* CWavefrontMtl::GetTexture(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext,
                                                    char* texName)
{
	if (texName == NULL)
		return NULL;

	ID3D11ShaderResourceView* srv = GetTexture(texName);
	if (srv != NULL)
		return srv;

	std::wstring str = string_cast<std::wstring>(texName);
	HRESULT hr = DirectX::CreateWICTextureFromFile(pD3DDevice, pD3DDeviceContext, str.c_str(), NULL, &srv);
	if (FAILED(hr))
		return NULL;

	m_Texture[m_countTexture] = srv;
	strcpy_s(m_texFileName[m_countTexture], sizeof(m_texFileName[m_countTexture]), texName);
	++m_countTexture;

	return srv;
}

WFOBJ_MTL* CWavefrontMtl::GetMaterial(char* mtlName)
{
	if (mtlName == NULL)
		return NULL;
	for (int i = 0; i < m_countMaterial; ++i)
	{
		if (strcmp(mtlName, m_Material[i].name) == 0)
			return &m_Material[i];
	}
	return NULL;
}

bool CWavefrontMtl::Load(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, char* mtlfilename,
                         char* deffilename)
{
	Cleanup();

	if (mtlfilename == NULL || mtlfilename[0] == NULL)
		return true;
	strcpy_s(m_MaterialFileName, sizeof(m_MaterialFileName), mtlfilename);

	// **********************************************************
	// MTL�t�@�C���̓ǂݍ���
	FILE* fileMtl;
	if (fopen_s(&fileMtl, m_MaterialFileName, "rt") != 0)
		return false;

	bool ret = LoadWavefrontMTL(fileMtl, m_Material, _countof(m_Material), m_countMaterial);

	fclose(fileMtl);

	if (ret == false)
	{
		Cleanup();
		return false;
	}

	// **********************************************************
	// �e�N�X�`���̓ǂݍ���
	for (int i = 0; (i < m_countMaterial) && (m_countTexture < _countof(m_Texture)); ++i)
	{
		char* tfname[] = {
			m_Material[i].map_Kd, m_Material[i].map_Ks, m_Material[i].map_Ka, m_Material[i].map_Bump,
			m_Material[i].map_D, m_Material[i].refl
		};
		for (int h = 0; (h < _countof(tfname)) && (m_countTexture < _countof(m_Texture)); ++h)
		{
			if (tfname[h][0] == NULL)
				continue;
			if (GetTexture(pD3DDevice, pD3DDeviceContext, tfname[h]) == NULL)
			{
				Cleanup();
				return false;
			}
		}
	}

	// **********************************************************
	// �f�t�H���g�ݒ�
	if (deffilename != NULL)
	{
		std::wstring str = string_cast<std::wstring>(deffilename);
		HRESULT hr = DirectX::CreateWICTextureFromFile(pD3DDevice, pD3DDeviceContext, str.c_str(), NULL,
		                                               &m_DefaultTexture);
		if (FAILED(hr))
		{
			Cleanup();
			return false;
		}
	}

	return true;
}

void CWavefrontMtl::Cleanup()
{
	m_MaterialFileName[0] = NULL;
	if (m_DefaultTexture != NULL)
	{
		m_DefaultTexture->Release();
		m_DefaultTexture = NULL;
	}
	for (int i = 0; i < m_countTexture; ++i)
	{
		if (m_Texture[i] != NULL)
		{
			m_Texture[i]->Release();
			m_Texture[i] = NULL;
		}
	}
	m_countTexture = 0;
	m_countMaterial = 0;
}