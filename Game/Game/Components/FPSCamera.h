#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class FPSCamera : public Component
{
public:
	static constexpr const char* Identifier = "FPSCamera";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	// â°âÒì]
	float m_yAngle = 0;
	// ècâÒì]
	float m_xAngle = 0;
	// â°âÒì]
	float m_yAngleLast = m_yAngle;
	// ècâÒì]
	float m_xAngleLast = m_xAngle;

public:
	void Start();
	void OnDestroy();
	void OnPreRender(GameCamera& camera);
	void Move(float dx, float dy);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(m_yAngle), CEREAL_OPTIONAL_NVP(m_xAngle), CEREAL_OPTIONAL_NVP(m_yAngleLast), CEREAL_OPTIONAL_NVP(m_xAngleLast));
	}

	void EditorGui();
};

