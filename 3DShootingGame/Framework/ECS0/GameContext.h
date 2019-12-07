#pragma once
#include <Common/StepTimer.h>
#include <Common/DeviceResources.h>
#include <Framework/ObjectField.h>
#include <Utilities/TypeId.h>

class Scene;
class GameObject;
class SceneManager;
class GameCamera;
class PhysXManager;
class PhysXScene;
class ImGuiManager;
class SaveHandler;
class PauseHandler;

// GameContext�C���^�[�t�F�C�X
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
		};

		template<typename T>
		struct ContextHolder : public IContextHolder
		{
			virtual T* Get() const = 0;
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

#ifndef GAMECONTEXT_USE_MAP
		template<typename T>
		struct sparse_set
		{
			std::vector<std::unique_ptr<T>> reserve;

			bool managed(type_id_t id)
			{
				return id < reserve.size() && reserve[id];
			}

			void assure(type_id_t id)
			{
				if (!(id < reserve.size()))
					reserve.resize(id + 1);
			}

			void put(type_id_t id, std::unique_ptr<T>&& value)
			{
				assure(id);
				reserve[id] = std::move(value);
			}

			std::unique_ptr<T>& get(type_id_t id)
			{
				if (managed(id))
					return reserve[id];
				return nullptr;
			}

			std::unique_ptr<T>& getfast(type_id_t id)
			{
				return reserve[id];
			}
		};
		sparse_set<IContextHolder> m_contexts;

		template<typename T> T* Get()
		{
			auto key = type_id<T>();
			if (m_contexts.managed(key))
				return static_cast<ContextHolder<T>*>(m_contexts.getfast(key).get())->Get();
			return nullptr;
		}

		template<typename T> void Register(std::unique_ptr<T>&& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextUniquePtrHolder<T>>(std::move(context))); }
		template<typename T> void Register(const std::shared_ptr<T>& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextSharedPtrHolder<T>>(context)); }
		template<typename T> void Register(const std::weak_ptr<T>& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextWeakPtrHolder<T>>(context)); }
		template<typename T> void Register(const std::reference_wrapper<T>& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextRefHolder<T>>(context)); }
		template<typename T> void Register(T& context) { m_contexts.put(type_id<T>(), std::make_unique<ContextRefHolder<T>>(context)); }
		template<typename T> void Register(T* context) { m_contexts.put(type_id<T>(), std::make_unique<ContextPtrHolder<T>>(context)); }
#else
		std::unordered_map<type_id_t, std::unique_ptr<IContextHolder>> m_contexts;

		template<typename T> T* Get()
		{
			auto key = type_id<T>();
			if (m_contexts.count(key) > 0)
			{
				auto holder = m_contexts.at(key).get();
				return dynamic_cast<ContextHolder<T>*>(holder)->Get();
			}
			return nullptr;
		}

		template<typename T> void Register(std::unique_ptr<T>&& context) { m_contexts[type_id<T>()] = std::make_unique<ContextUniquePtrHolder<T>>(std::move(context)); }
		template<typename T> void Register(const std::shared_ptr<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextSharedPtrHolder<T>>(context); }
		template<typename T> void Register(const std::weak_ptr<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextWeakPtrHolder<T>>(context); }
		template<typename T> void Register(const std::reference_wrapper<T>& context) { m_contexts[type_id<T>()] = std::make_unique<ContextRefHolder<T>>(context); }
		template<typename T> void Register(T& context) { m_contexts[type_id<T>()] = std::make_unique<ContextRefHolder<T>>(context); }
		template<typename T> void Register(T* context) { m_contexts[type_id<T>()] = std::make_unique<ContextPtrHolder<T>>(context); }
#endif
	};

	ContextLocator m_locator;

public:
	virtual GameContext* GetParent()
	{
		return nullptr;
	}

	template<typename T>
	T* GetPtr()
	{
		auto context = m_locator.Get<T>();
		if (context == nullptr)
		{
			auto parent = GetParent();
			if (parent != nullptr)
				context = parent->GetPtr<T>();
		}
		return context;
	}

	template<typename T>
	T& Get()
	{
		auto context = GetPtr<T>();
		if (context == nullptr)
			throw std::exception("missing service.");
		return *context;
	}

	template<typename T>
	void Register(T&& context)
	{
		m_locator.Register(std::forward<T>(context));
	}

	// DeviceResource�擾
	virtual DX::DeviceResources& GetDR() { return Get<DX::DeviceResources>(); };
	// �^�C�}�[�擾
	virtual DX::StepTimer& GetTimer() { return Get<DX::StepTimer>(); };
	// �J�����擾
	virtual GameCamera& GetCamera() { return Get<GameCamera>(); };
	// �R�����X�e�[�g�擾
	virtual DirectX::CommonStates& GetStates() { return Get<DirectX::CommonStates>(); };
	// �G�t�F�N�g�t�@�N�g���[�擾
	virtual DirectX::EffectFactory& GetEffectFactory() { return Get<DirectX::EffectFactory>(); };
	// �V�[�����擾
	virtual Scene& GetScene() = 0;
	// �V�[���}�l�[�W�����擾
	virtual SceneManager& GetSceneManager() { return Get<SceneManager>(); };
	// �����}�l�[�W�����擾
	virtual PhysXManager& GetPhysics() { return Get<PhysXManager>(); };
	// �����V�[�����擾
	virtual PhysXScene& GetPhysicsScene() { return Get<PhysXScene>(); };
	// GUI�}�l�[�W��
	virtual ImGuiManager& GetGuiManager() { return Get<ImGuiManager>(); };
	// �Z�[�u�n���h��
	virtual SaveHandler& GetSaveHandler() { return Get<SaveHandler>(); };
	// �|�[�Y�n���h��
	virtual PauseHandler& GetPauseHandler() { return Get<PauseHandler>(); };
};