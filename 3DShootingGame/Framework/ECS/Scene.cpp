#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Serialize.h"
#include <Framework/Components/AllComponents.h>

GameObject Scene::Create()
{
	return GameObject{ &registry, registry.create() };
}

Scene::Scene()
{
	Components::InitializeDependency(registry);
}

Scene::~Scene()
{
	registry.reset();
}

bool Scene::Load()
{
	registry.reset();
	return Components::LoadScene(location, registry, [](auto& registry) {
		Components::InitializeDependency(registry);
		Components::InitializeLifecycleEvents(registry);
		});
}

bool Scene::Save() const
{
	return Components::SaveScene(location, registry);
}
