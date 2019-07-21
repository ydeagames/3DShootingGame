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
	ObjectField<GameObject> field = obj;
	addingObjects.emplace_back(std::move(obj));
	return field;
}

void Scene::Initialize(GameContext& context)
{
	physics = context.GetPhysics().CreateScene();

	for (auto& object : gameObjects)
		object->Initialize(context);
}

void Scene::Update(GameContext& context)
{
	for (auto& object : addingObjects)
		object->Initialize(context);
	gameObjects.splice(gameObjects.end(), std::move(addingObjects));

	for (auto& object : gameObjects)
		object->Update(context);

	for (auto itr = gameObjects.begin(); itr != gameObjects.end();)
	{
		auto& object = *itr;
		if (object->destroyed)
		{
			object->Finalize(context);
			itr = gameObjects.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void Scene::Render(GameContext& context)
{
	for (auto& object : gameObjects)
		object->Render(context);
}

void Scene::Finalize(GameContext& context)
{
	for (auto& object : gameObjects)
		object->Finalize(context);
}

