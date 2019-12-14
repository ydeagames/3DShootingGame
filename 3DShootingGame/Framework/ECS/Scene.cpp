#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Serialize.h"
#include "Project.h"
#include <Utilities/WindowsUtils.h>

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

bool Scene::Load()
{
	registry.reset();
	context = {};
	return ECS::AllComponents::LoadScene(info.location, registry, [](auto& registry) {
		ECS::AllComponents::InitializeDependency(registry);
		ECS::AllComponents::InitializeLifecycleEvents(registry);
		});
}

bool Scene::Save() const
{
	return ECS::AllComponents::SaveScene(info.location, registry);
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
