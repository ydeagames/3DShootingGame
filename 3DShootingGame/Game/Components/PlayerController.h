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
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureArrow;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureCircle;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureBase;

	// カメラ
	GameCamera* m_camera = nullptr;

	// 始点
	DirectX::SimpleMath::Vector3 m_beginDrag;
	// 終点
	DirectX::SimpleMath::Vector3 m_endDrag;

	// ドラッグ中
	bool m_dragging = false;

public:
	void Start();
	void Update();
	void RenderStart();
	void Render(GameCamera& camera);
	void RenderGui(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(power), CEREAL_OPTIONAL_NVP(sensitivity));
	}

	void EditorGui();
};
