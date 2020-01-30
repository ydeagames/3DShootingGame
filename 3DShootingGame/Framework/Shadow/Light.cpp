#include "pch.h"
#include "Light.h"
#include <Utilities/Math3DUtils.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

Light::Light()
{
	SetLook(Vector3(3.0f, 3.0f, -3.0f), Vector3(-1.f, -1.f, 1.f), Vector3::Up);
	SetProjection(Matrix::CreateOrthographic(
		30,
		30,
		.01f,
		200.0f
	));
}

void Light::SetLook(const DirectX::SimpleMath::Vector3& positionVec, const DirectX::SimpleMath::Vector3& directionVec, const DirectX::SimpleMath::Vector3& upVec)
{
	position = positionVec;
	direction = Math3DUtils::Normalized(directionVec);
	up = upVec;
	view = Matrix::CreateLookAt(position, position + direction, up);
}

void Light::SetPosition(const DirectX::SimpleMath::Vector3& positionVec)
{
	position = positionVec;
	view = Matrix::CreateLookAt(position, position + direction, up);
}

void Light::SetProjection(const DirectX::SimpleMath::Matrix& proj)
{
	projection = proj;
}
