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
	float stopSpeed = 1;
	float lineGravity = 9.8f;
	int lineCount = 12;
	int lineCountDiv = 2;
	float linePowerScale = .5f;
	float powerTime = -1;
	float percent = 0;
	float speed = 1;

private:
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// 球
	std::unique_ptr<DirectX::GeometricPrimitive> m_sphereModel;

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

	// 点
	DirectX::SimpleMath::Vector3 m_lastPos;
	DirectX::SimpleMath::Quaternion m_lastRot;

	// ドラッグ中
	bool m_dragging = false;
	// 移動可能
	bool m_movable = true;
	// 移動中
	bool m_moving = false;

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
		archive(CEREAL_OPTIONAL_NVP(power), CEREAL_OPTIONAL_NVP(sensitivity), CEREAL_OPTIONAL_NVP(stopSpeed));
		archive(CEREAL_OPTIONAL_NVP(lineGravity), CEREAL_OPTIONAL_NVP(lineCount), CEREAL_OPTIONAL_NVP(lineCountDiv), CEREAL_OPTIONAL_NVP(linePowerScale));
		archive(CEREAL_OPTIONAL_NVP(powerTime), CEREAL_OPTIONAL_NVP(percent), CEREAL_OPTIONAL_NVP(speed));
	}

	void EditorGui();
};
