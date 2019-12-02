#pragma once

class GameObject;

class Scene final
{
public:
	std::string name;
	std::string location;
	entt::registry registry;

public:
	GameObject Create();
	Scene();
	~Scene();
	bool Load();
	bool Save() const;
};

