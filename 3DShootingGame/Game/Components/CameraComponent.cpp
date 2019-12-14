#include "pch.h"
#include "CameraComponent.h"
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

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
