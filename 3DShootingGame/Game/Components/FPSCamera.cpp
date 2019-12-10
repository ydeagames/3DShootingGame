#include "pch.h"
#include "FPSCamera.h"
#include <Common/DeviceResources.h>
#include <Framework/ECS/GameContext.h>
#include <Utilities/Input.h>
#include <Utilities/MathUtils.h>
#include <Framework/Components/Transform.h>
#include <Framework/Context/GameCamera.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void FPSCamera::Start()
{
	Input::SetMouseMode(Mouse::Mode::MODE_RELATIVE);
}

void FPSCamera::OnDestroy()
{
	//Input::SetMouseMode(Mouse::Mode::MODE_ABSOLUTE);
}

void FPSCamera::Update()
{
	if (!Input::GetMouseButton(Input::Buttons::MouseRight))
		if (Input::GetMouseMode() == DirectX::Mouse::Mode::MODE_RELATIVE)
		{
			auto move = Input::GetMousePosition();
			Move(float(move.x), float(move.y));
		}

	m_xAngle = MathUtils::Clamp(m_xAngle, -XM_PIDIV2, XM_PIDIV2);

	m_xAngleLast = MathUtils::Lerp(.25f, m_xAngleLast, m_xAngle);
	m_yAngleLast = MathUtils::Lerp(.25f, m_yAngleLast, m_yAngle);

	// ÉrÉÖÅ[çsóÒÇéZèoÇ∑ÇÈ
	Quaternion rotX = Quaternion::CreateFromAxisAngle(Vector3::UnitX, -m_xAngleLast);
	Quaternion rotY = Quaternion::CreateFromAxisAngle(Vector3::UnitY, -m_yAngleLast);
	Quaternion rot = -rotX * rotY;

	if (cameraptr)
	{
		gameObject.GetComponent<Transform>().localRotation = rot;
	}

	CameraComponent::Update();
}

void FPSCamera::Move(float dx, float dy)
{
	if (dx != 0.0f || dy != 0.0f)
	{
		// Çxé≤ÇÃâÒì]
		float yAngle = XMConvertToRadians(dx);
		// Çwé≤ÇÃâÒì]
		float xAngle = XMConvertToRadians(dy);

		m_xAngle += xAngle;
		m_yAngle += yAngle;
	}
}

void FPSCamera::EditorGui()
{
	ImGui::InputFloat("yAngle#FPSCamera", &m_yAngle);
	ImGui::InputFloat("xAngle#FPSCamera", &m_yAngle);
	ImGui::InputFloat("yAngleLast#FPSCamera", &m_yAngleLast);
	ImGui::InputFloat("xAngleLast#FPSCamera", &m_yAngleLast);
}
