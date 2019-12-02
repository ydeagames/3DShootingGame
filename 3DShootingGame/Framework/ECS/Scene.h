#pragma once

class GameObject;

class SceneInfo final
{
public:
	std::string name;
	std::string location;
};

class Scene final
{
public:
	SceneInfo info;
	entt::registry registry;

public:
	GameObject Create();
	Scene();
	~Scene();
	bool Load();
	bool Save() const;
};

