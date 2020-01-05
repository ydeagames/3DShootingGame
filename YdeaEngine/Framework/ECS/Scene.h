#pragma once
#include "Context.h"

class GameObject;

class SceneInfo final
{
public:
	static constexpr const char* SceneExtension = "scene.json";

public:
	std::string name;
	std::string location;

public:
	SceneInfo(const std::string& name, const std::string& location);
	static SceneInfo CreateFromLocation(const std::string& location);
	static SceneInfo CreateFromName(const std::string& name);

public:
	bool Valid();
};

class Scene final
{
public:
	SceneInfo info;
	entt::registry registry;
	Context context;
	bool destroyed = false;

public:
	Scene(const SceneInfo& info);
	Scene();
	~Scene();
	bool Load(const std::string& location);
	bool Load();
	bool Save(const std::string& location) const;
	bool Save() const;
	GameObject NullGameObject();

public:
	static void Destroy(Scene* scene);
	static void Destroy(Scene& scene);
};

