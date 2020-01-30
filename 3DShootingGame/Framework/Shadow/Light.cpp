#include "pch.h"
#include "Light.h"
#include <Utilities/Math3DUtils.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

Light::Light()
{
	SetLookAt(Vector3(3.0f, 3.0f, -3.0f), Vector3::Zero, Vector3::Up);
	SetProjection(Matrix::CreateOrthographic(
		30,
		30,
		.01f,
		500.0f
	));
}

void Light::SetLookAt(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& target, const DirectX::SimpleMath::Vector3& up)
{
	position = pos;
	direction = Math3DUtils::Normalized(target - pos);
	view = Matrix::CreateLookAt(pos, target, up);
}

void Light::SetProjection(const DirectX::SimpleMath::Matrix& proj)
{
	projection = proj;
}
