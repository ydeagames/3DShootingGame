#pragma once
#include <Framework/ECS/Component.h>
#include <Framework/Effekseer/EffekseerManager.h>

class GameContext;
class GameCamera;

class EffekseerEffect : public Component
{
public:
	static constexpr const char* Identifier = "EffekseerEffect";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

private:
	// エフェクト
	effekseer_unique_ptr<Effekseer::Effect> m_effect;

public:
	// パス
	std::string path;

	// PlayOnAwake
	bool playOnAwake;

public:
	// 描画初期化
	void RenderStart();
	// エフェクト再生
	Effekseer::Handle Play(const DirectX::SimpleMath::Vector3& pos);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(path), CEREAL_OPTIONAL_NVP(playOnAwake));
	}

	void EditorGui();
};
