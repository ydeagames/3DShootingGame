#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Serialize.h"
#include <Framework/Components/AllComponents.h>

bool SceneInfo::Valid()
{
	std::ifstream file(location);
	return file.good();
}

GameObject Scene::Create()
{
	return GameObject{ &registry, registry.create() };
}

Scene::Scene()
{
	ECS::AllComponents::InitializeDependency(registry);
}

Scene::~Scene()
{
	registry.reset();
}

bool Scene::Load()
{
	registry.reset();
	return ECS::AllComponents::LoadScene(info.location, registry, [](auto& registry) {
		ECS::AllComponents::InitializeDependency(registry);
		ECS::AllComponents::InitializeLifecycleEvents(registry);
		});
}

bool Scene::Save() const
{
	return ECS::AllComponents::SaveScene(info.location, registry);
}
