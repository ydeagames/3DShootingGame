#pragma once
#include <Common/StepTimer.h>
#include <Common/DeviceResources.h>

class Scene;
class GameObject;
class SceneManager;
class GameCamera;
class PhysXManager;

// GameContextインターフェイス
class GameContext
{
public:
	GameContext() = default;
	virtual ~GameContext() = default;

public:
	// DeviceResource取得
	virtual DX::DeviceResources& GetDR() = 0;
	// タイマー取得
	virtual DX::StepTimer& GetTimer() = 0;
	// カメラ取得
	virtual GameCamera& GetCamera() = 0;
	// コモンステート取得
	virtual DirectX::CommonStates& GetStates() = 0;
	// エフェクトファクトリー取得
	virtual DirectX::EffectFactory& GetEffectFactory() = 0;
	// シーンを取得
	virtual Scene& GetScene() = 0;
	// シーンにオブジェクトを追加
	virtual GameContext& operator<<(const std::shared_ptr<GameObject>& obj) = 0;
	// シーンマネージャを取得
	virtual SceneManager& GetSceneManager() = 0;
	// 物理マネージャを取得
	virtual PhysXManager& GetPhysics() = 0;
};
