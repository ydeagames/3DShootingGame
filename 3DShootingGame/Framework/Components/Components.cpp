#include "pch.h"
#include "Components.h"
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PrimitiveRenderer::RenderStart()
{
	m_model = GeometricPrimitive::CreateTeapot(GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext());
}

void PrimitiveRenderer::Render(GameCamera& camera)
{
	if (m_model)
		m_model->Draw(gameObject.GetComponent<Transform>().GetMatrix(), camera.view, camera.projection);
}

void MoveUpdater::Start()
{
	vel += Vector3::Right * .1f;
}

void MoveUpdater::Update()
{
	gameObject.GetComponent<Transform>().position += vel;
}

void MoveDownUpdater::Update()
{
	auto pos = gameObject.GetComponent<Transform>().position;
	pos.y -= .1f;
	gameObject.GetComponent<Transform>().position = pos;
}

void CameraComponent::Render(GameCamera& camera)
{
	cameraptr = &camera;
}

void CameraComponent::Update()
{
	if (cameraptr)
	{
		Vector3 s, t;
		Quaternion r;
		gameObject.GetComponent<Transform>().GetMatrix().Decompose(s, r, t);
		cameraptr->view = Matrix::CreateScale(Vector3::One / s) * DirectX::SimpleMath::Matrix::CreateLookAt(t,
			t + Vector3::Transform(Vector3::Forward, Matrix::CreateFromQuaternion(r)),
			Vector3::Transform(Vector3::Up, Matrix::CreateFromQuaternion(r)));
	}
}
