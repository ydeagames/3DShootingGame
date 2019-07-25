#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameContext.h"
#include <Framework/PhysX/PhysXScene.h>
#include <Framework/PhysX/PhysXManager.h>

Scene::Scene()
{
}

Scene::~Scene()
{
}

ObjectField<GameObject> Scene::Add(ObjectHolder<GameObject>&& obj)
{
	auto field = ObjectField<GameObject>(obj);
	m_addingObjects.emplace_back(std::move(obj));
	m_objectMap.insert(std::make_pair(field->GetName(), field));
	return field;
}

ObjectField<GameObject> Scene::Find(const std::wstring& key)
{
	if (m_objectMap.count(key) > 0)
	{
		for (auto itr = m_objectMap.find(key); itr != m_objectMap.end();)
		{
			auto& component = (*itr).second;
			if (component && !component->IsDestroyed())
				return component;
			itr = m_objectMap.erase(itr);
		}
	}
	return nullptr;
}

std::list<ObjectField<GameObject>> Scene::FindAll(const std::wstring& key)
{
	std::list<ObjectField<GameObject>> result;
	if (m_objectMap.count(key) > 0)
	{
		for (auto itr = m_objectMap.find(key); itr != m_objectMap.end();)
		{
			auto& component = (*itr).second;
			if (component && !component->IsDestroyed())
			{
				result.push_back(component);
				++itr;
			}
			else
			{
				itr = m_objectMap.erase(itr);
			}
		}
	}
	return result;
}

ObjectField<GameObject> Scene::AddGameObject(const std::wstring& objname)
{
	return Add(GameObject::Create(objname));
}

void Scene::Initialize(GameContext& context)
{
	physics = context.GetPhysics().CreateScene();

	for (auto& object : m_gameObjects)
		object->Initialize(context);
}

void Scene::Update(GameContext & context)
{
	for (auto& object : m_addingObjects)
		object->Initialize(context);
	m_gameObjects.splice(m_gameObjects.end(), std::move(m_addingObjects));

	for (auto& object : m_gameObjects)
		object->Update(context);

	for (auto itr = m_gameObjects.begin(); itr != m_gameObjects.end();)
	{
		auto& object = *itr;
		if (object->IsDestroyed())
		{
			object->Finalize(context);
			itr = m_gameObjects.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void Scene::Render(GameContext & context)
{
	for (auto& object : m_gameObjects)
		object->Render(context);
}

void Scene::Finalize(GameContext & context)
{
	for (auto& object : m_gameObjects)
		object->Finalize(context);
}

