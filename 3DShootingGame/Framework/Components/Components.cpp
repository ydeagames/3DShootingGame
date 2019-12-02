#include "pch.h"
#include "Components.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PrimitiveRenderer::RenderStart()
{
	m_model = GeometricPrimitive::CreateTeapot(GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext());
}

void PrimitiveRenderer::Render(Camera& camera)
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
	gameObject.GetComponent<Transform>().position.y -= .1f;
}

void CameraComponent::Render(Camera& camera)
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
