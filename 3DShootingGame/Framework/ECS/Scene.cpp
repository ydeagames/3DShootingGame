// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2020-01-09 18:30:53 +0900

#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Serialize.h"
#include "Project.h"
#include <Utilities/WindowsUtils.h>

SceneInfo::SceneInfo()
	: name("")
	, location("")
{
}

SceneInfo::SceneInfo(const std::string& name, const std::string& location)
	: name(name)
	, location(location)
{
}

SceneInfo SceneInfo::CreateFromLocation(const std::string& location)
{
	return SceneInfo(WindowsUtils::GetFileName(location, SceneExtension), location);
}

SceneInfo SceneInfo::CreateFromName(const std::string& name)
{
	return SceneInfo(name, name + "." + SceneExtension);
}

bool SceneInfo::Valid()
{
	std::ifstream file(location);
	return file.good();
}

Scene::Scene(const SceneInfo& info)
	: info(info)
{
	ECS::AllComponents::InitializeDependency(registry);
}

Scene::Scene()
	: Scene(SceneInfo::CreateFromName("Untitled"))
{
}

Scene::~Scene()
{
	registry.reset();
	context = {};
}

bool Scene::Load(const std::string& location)
{
	registry.reset();
	context = {};
	return ECS::AllComponents::LoadScene(location, registry, [](auto& registry) {
		ECS::AllComponents::InitializeDependency(registry);
		ECS::AllComponents::InitializeLifecycleEvents(registry);
		});
}

bool Scene::Load()
{
	registry.reset();
	context = {};
	auto action = info.action;
	return ECS::AllComponents::LoadScene(info.location, registry, [action](auto& registry) {
		ECS::AllComponents::InitializeDependency(registry);
		ECS::AllComponents::InitializeLifecycleEvents(registry);
		if (action)
			action(registry);
		});
}

bool Scene::Save(const std::string& location) const
{
	return ECS::AllComponents::SaveScene(location, registry);
}

bool Scene::Save() const
{
	return Save(info.location);
}

GameObject Scene::NullGameObject()
{
	return GameObject(&registry, entt::null);
}

void Scene::Destroy(Scene* scene)
{
	if (scene)
		Destroy(*scene);
}

void Scene::Destroy(Scene& scene)
{
	scene.destroyed = true;
}


