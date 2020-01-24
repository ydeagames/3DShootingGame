#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class ShootGauge : public Component
{
public:
	static constexpr const char* Identifier = "ShootGauge";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string texture;
	float time = 0;
	float percent = 0;
	float speed = 0;

private:
	// �G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// �v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// �C���v�b�g���C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void Update();
	void RenderStart();
	void RenderGui(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(texture), CEREAL_OPTIONAL_NVP(time), CEREAL_OPTIONAL_NVP(percent), CEREAL_OPTIONAL_NVP(speed));
	}

	void EditorGui();
};