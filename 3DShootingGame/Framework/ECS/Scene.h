#pragma once

class GameObject;

class SceneInfo final
{
public:
	std::string name;
	std::string location;

public:
	bool Valid();
};

class Scene final
{
public:
	SceneInfo info;
	entt::registry registry;

public:
	GameObject Create();
	Scene(const SceneInfo& info);
	Scene();
	~Scene();
	bool Load();
	bool Save() const;
};

