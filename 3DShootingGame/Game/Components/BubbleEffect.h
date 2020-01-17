#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class BubbleEffect : public Component
{
public:
	static constexpr const char* Identifier = "BubbleEffect";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string texture;
	float time = 0;
	int count;
	int seed;

private:
	// �G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// �C���v�b�g���C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void Update();
	void RenderStart();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(texture), CEREAL_OPTIONAL_NVP(time), CEREAL_OPTIONAL_NVP(count), CEREAL_OPTIONAL_NVP(seed));
	}

	void EditorGui();
};