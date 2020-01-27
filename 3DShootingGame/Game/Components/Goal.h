#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class Goal : public Component
{
public:
	static constexpr const char* Identifier = "Goal";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
		component.DependsOn<Rigidbody>();
	}

public:
	// 追尾ターゲット
	entt::entity target = entt::null;

private:
	bool m_goaled = false;

public:
	void Start();
	void Update();
	void OnDestroy();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(target));
	}

	template<class Reference>
	void Reference(Reference& reference)
	{
		reference(target);
	}

	void EditorGui();
};
