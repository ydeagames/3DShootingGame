#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameContext.h"
#include "PhysXScene.h"
#include "PhysXManager.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Add(const std::shared_ptr<GameObject>& obj)
{
	addingObjects.push_back(obj);
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

	gameObjects.remove_if(std::mem_fn(&GameObject::destroyed));
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

