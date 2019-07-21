#pragma once
#include <Common/StepTimer.h>
#include <Common/DeviceResources.h>
#include <Utilities/TypeId.h>

class Scene;
class GameObject;
class SceneManager;
class GameCamera;
class PhysXManager;
class ImGuiManager;
class SaveHandler;
class PauseHandler;

// GameContextインターフェイス
class GameContext
{
public:
	GameContext() = default;
	virtual ~GameContext() = default;

private:
	std::unordered_map<type_id_t, std::unique_ptr<void>> m_contexts;

public:
	template<typename T>
	T& Get()
	{
		return *dynamic_cast<T*>(m_contexts[type_id<T>()].get());
	}

	template<typename T>
	void Register(std::unique_ptr<T>&& context)
	{
		m_contexts[type_id<T>()] = std::move(context);
	}

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
	// ウィンドウを取得
	virtual HWND& GetWindowHandle() = 0;
	// GUIマネージャ
	virtual ImGuiManager& GetGuiManager() = 0;
	// セーブハンドラ
	virtual SaveHandler& GetSaveHandler() = 0;
	// ポーズハンドラ
	virtual PauseHandler& GetPauseHandler() = 0;
};
