#pragma once
#include <Framework/ECS/Component.h>
#include "CameraComponent.h"

class GameCamera;

class FPSCamera : public CameraComponent
{
public:
	static constexpr const char* Identifier = "FPSCamera";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	// ����]
	float m_yAngle = 0;
	// �c��]
	float m_xAngle = 0;
	// ����]
	float m_yAngleLast = m_yAngle;
	// �c��]
	float m_xAngleLast = m_xAngle;

public:
	void Start();
	void OnDestroy();
	void Update();
	void Move(float dx, float dy);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(m_yAngle), CEREAL_OPTIONAL_NVP(m_xAngle), CEREAL_OPTIONAL_NVP(m_yAngleLast), CEREAL_OPTIONAL_NVP(m_xAngleLast));
	}

	void EditorGui();
};

