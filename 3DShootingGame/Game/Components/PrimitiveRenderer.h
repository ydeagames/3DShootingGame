#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class PrimitiveRenderer : public Component
{
public:
	static constexpr const char* Identifier = "PrimitiveRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string texture = "Resources/Textures/Title/Background.png";

private:
	// �G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// ���f��
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;

	// �C���v�b�g���C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void RenderStart();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(texture));
	}

	void EditorGui();
};