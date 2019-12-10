#include "pch.h"
#include "PrimitiveRenderer.h"
#include <Common/DeviceResources.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Components/Transform.h>
#include <Framework/Context/GameCamera.h>
#include <Utilities/StringCast.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PrimitiveRenderer::RenderStart()
{
	auto& dr = GameContext::Get<DX::DeviceResources>();

	m_model = GeometricPrimitive::CreateTeapot(GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext());
	// m_model = GeometricPrimitive::CreateGeoSphere(dr.GetD3DDeviceContext(), 1.f, 3U, false);

	auto texture_str = string_cast<std::wstring>(texture);

	if (FAILED(CreateWICTextureFromFile(
		dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
		texture_str.c_str(), nullptr, m_texture.ReleaseAndGetAddressOf())))
		m_texture = nullptr;
	else
	{
		// �|���S���p�G�t�F�N�g�쐬
		m_basicEffect = std::make_unique<BasicEffect>(dr.GetD3DDevice());
		m_basicEffect->SetTextureEnabled(true);

		// ���C�g�L��
		m_basicEffect->SetLightingEnabled(true);
		// �����̐F��ݒ�
		m_basicEffect->SetAmbientLightColor(SimpleMath::Vector3(0.2f, 0.2f, 0.2f));
		// �g�U���ˌ��̑f�ސF��ݒ�
		m_basicEffect->SetDiffuseColor(SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

		// �V�F�[�_�[�擾
		m_model->CreateInputLayout(m_basicEffect.get(), m_pInputLayout.ReleaseAndGetAddressOf());
	}
}

void PrimitiveRenderer::Render(GameCamera& camera)
{
	if (m_model)
	{
		auto& dr = GameContext::Get<DX::DeviceResources>();
		auto& commonStates = GameContext::Get<CommonStates>();

		auto ctx = dr.GetD3DDeviceContext();
		// ���[���h�s��ݒ�
		m_basicEffect->SetWorld(gameObject.GetComponent<Transform>().GetMatrix());
		// �r���[�s��ݒ�
		m_basicEffect->SetView(camera.view);
		// �v���W�F�N�V�����s��ݒ�
		m_basicEffect->SetProjection(camera.projection);
		// �G�t�F�N�g�̐ݒ�
		m_basicEffect->Apply(ctx);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�
		ctx->OMSetDepthStencilState(commonStates.DepthDefault(), 0);
		// �u�����h�X�e�[�g�̐ݒ�
		ctx->OMSetBlendState(commonStates.AlphaBlend(), nullptr, 0xffffffff);
		// ���X�^���C�U�X�e�[�g��ݒ�
		ctx->RSSetState(commonStates.CullClockwise());
		// ���̓��C�A�E�g�̐ݒ�
		ctx->IASetInputLayout(m_pInputLayout.Get());

		m_basicEffect->SetTexture(m_texture.Get());

		m_model->Draw(m_basicEffect.get(), m_pInputLayout.Get());
		//m_model->Draw(gameObject->transform->GetMatrix(), context.GetCamera().view, context.GetCamera().projection, Colors::White, m_texture.Get(), false);
	}
}

void PrimitiveRenderer::EditorGui()
{
	std::string tmpname = texture;
	tmpname.resize(128);
	ImGui::InputText("Texture##Background", &tmpname[0], tmpname.size());
	auto tmpname2 = std::string(tmpname.c_str());
	if (texture != tmpname2)
	{
		texture = tmpname2;
		RenderStart();
	}
}