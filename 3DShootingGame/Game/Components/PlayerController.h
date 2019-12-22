#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;
class Rigidbody;

class PlayerController : public Component
{
public:
	static constexpr const char* Identifier = "PlayerController";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
		component.DependsOn<Rigidbody>();
	}

public:
	float power = 1;
	float sensitivity = 1;

private:
	// �G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// �v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// �C���v�b�g���C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

public:
	void Start();
	void Update();
	void RenderStart();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(power), CEREAL_OPTIONAL_NVP(sensitivity));
	}

	void EditorGui();
};