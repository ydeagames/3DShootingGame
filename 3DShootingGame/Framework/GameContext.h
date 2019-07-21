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
	struct ContextLocator
	{
		struct IContextHolder
		{
			virtual ~IContextHolder() = default;
			virtual void* GetPtr() const = 0;
		};

		template<typename T>
		struct ContextHolder : public IContextHolder
		{
			virtual T* Get() const = 0;
			void* GetPtr() const override { return Get(); }
		};

		template<typename T>
		struct ContextUniquePtrHolder : public ContextHolder<T>
		{
			std::unique_ptr<T> m_context;
			ContextUniquePtrHolder(std::unique_ptr<T>&& context) : m_context(std::move(context)) {}
			T* Get() const override { return m_context.get(); }
		};

		template<typename T>
		struct ContextSharedPtrHolder : public ContextHolder<T>
		{
			std::shared_ptr<T> m_context;
			ContextSharedPtrHolder(const std::shared_ptr<T>& context) : m_context(context) {}
			T* Get() const override { return m_context.get(); }
		};

		template<typename T>
		struct ContextWeakPtrHolder : public ContextHolder<T>
		{
			std::weak_ptr<T> m_context;
			ContextWeakPtrHolder(const std::weak_ptr<T>& context) : m_context(context) {}
			T* Get() const override { if (auto p = m_context.lock()) return p.get(); else return nullptr; }
		};

		template<typename T>
		struct ContextRefHolder : public ContextHolder<T>
		{
			std::reference_wrapper<T> m_context;
			ContextRefHolder(const std::reference_wrapper<T>& context) : m_context(context) {}
			ContextRefHolder(T& context) : m_context(std::ref(context)) {}
			T* Get() const override { return &m_context; }
		};

		template<typename T>
		struct ContextPtrHolder : public ContextHolder<T>
		{
			T* m_context;
			ContextPtrHolder(T* context) : m_context(context) {}
			T* Get() const override { return m_context; }
		};

		std::unordered_map<type_id_t, std::unique_ptr<IContextHolder>> m_contexts;

		template<typename T> T* Get() { return dynamic_cast<T*>(m_contexts[type_id<T>()].Get()); }

		template<typename T> void Register(std::unique_ptr<T>&& context) { m_contexts[type_id<T>()] = std::make_unique<ContextUniquePtrHolder<T>>(context); }
		template<typename T> void Register(const std::shared_ptr<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextSharedPtrHolder<T>>(context); }
		template<typename T> void Register(const std::weak_ptr<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextWeakPtrHolder<T>>(context); }
		template<typename T> void Register(const std::reference_wrapper<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextRefHolder<T>>(context); }
		template<typename T> void Register(T& context) { m_contexts[type_id<T>()] = std::make_unique<ContextRefHolder<T>>(context); }
		template<typename T> void Register(T* context) { m_contexts[type_id<T>()] = std::make_unique<ContextPtrHolder<T>>(context); }
	};

	ContextLocator m_locator;
	GameContext* m_parent;

public:
	template<typename T>
	T& Get()
	{
		auto context = m_locator.Get<T>();
		if (context == nullptr && m_parent != nullptr)
			context = m_parent->Get<T>();
		if (context == nullptr)
			throw std::exception("missing service.");
		return *context;
	}

	template<typename T>
	void Register(T&& context)
	{
		return m_locator.Register(std::forward<T>(context));
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
