#pragma once
#include <Framework/Context/SceneManager.h>
#include <Framework/ECS/Component.h>

class GameCamera;

namespace SceneTransitions
{
	// フェード実装
	class FadeTransition : public Component
	{
	public:
		SceneInfo info;

	public:
		float time = 0;
		float totalTime = 2;

	private:
		std::unique_ptr<DirectX::GeometricPrimitive> m_plane;
		bool m_changed = false;

	public:
		void RenderStart();
		void Update();
		void Render(GameCamera& camera);

	public:
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_OPTIONAL_NVP(time), CEREAL_OPTIONAL_NVP(totalTime));
		}

		void EditorGui();
	};

	// フェード実装
	class RuleTransition : public Component
	{
	public:
		SceneInfo info;

	public:
		float time = 0;
		float totalTime = 2;
		struct ConstBuffer;

	private:
		// プリミティブバッチ
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>	m_primitiveBatch;
		// インプットレイアウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		// 頂点シェーダ
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
		// ピクセルシェーダ
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
		// バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;
		// テクスチャハンドル
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;

		std::unique_ptr<DirectX::GeometricPrimitive> m_plane;
		bool m_changed = false;

	public:
		void RenderStart();
		void Update();
		void Render(GameCamera& camera);

	public:
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_OPTIONAL_NVP(time), CEREAL_OPTIONAL_NVP(totalTime));
		}

		void EditorGui();
	};

	SceneManager::Transition CreateFadeTransition(float duration);
	SceneManager::Transition CreateRuleTransition(float duration);
}
