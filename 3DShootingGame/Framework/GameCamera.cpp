#include "pch.h"
#include "GameCamera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

DirectX::SimpleMath::Vector3 GameCamera::GetPosition() const
{
	return Vector3::Transform(Vector3::Zero, view.Invert());
}

DirectX::SimpleMath::Ray GameCamera::ScreenPointToRay(const DirectX::SimpleMath::Vector3& point) const
{
	return ViewportPointToRay(ScreenToViewportPoint(point));
}

DirectX::SimpleMath::Ray GameCamera::ViewportPointToRay(const DirectX::SimpleMath::Vector3& point) const
{
	auto pointNear = Vector3(point.x, point.y, 0.0f);
	auto pointFar = Vector3(point.x, point.y, 1.0f);

	auto inverseviewproj = (view * projection).Invert();
	auto rayNear = Vector3::Transform(pointNear, inverseviewproj);
	auto rayFar = Vector3::Transform(pointFar, inverseviewproj);
	auto raySubtraction = rayFar - rayNear;
	auto rayDirection = raySubtraction;
	rayDirection.Normalize();

	return Ray(Vector3(rayNear), Vector3(rayDirection));
}

DirectX::SimpleMath::Vector3 GameCamera::WorldToScreenPoint(const DirectX::SimpleMath::Vector3& point) const
{
	return Vector3::Transform(point, view * projection * viewport);
}

DirectX::SimpleMath::Vector3 GameCamera::WorldToViewportPoint(const DirectX::SimpleMath::Vector3& point) const
{
	return Vector3::Transform(point, view * projection);
}

DirectX::SimpleMath::Vector3 GameCamera::ViewportToWorldPoint(const DirectX::SimpleMath::Vector3& point) const
{
	return Vector3::Transform(point, (view * projection).Invert());
}

DirectX::SimpleMath::Vector3 GameCamera::ScreenToWorldPoint(const DirectX::SimpleMath::Vector3& point) const
{
	return Vector3::Transform(point, (view * projection * viewport).Invert());
}

DirectX::SimpleMath::Vector3 GameCamera::ScreenToViewportPoint(const DirectX::SimpleMath::Vector3& point) const
{
	return Vector3::Transform(point, viewport.Invert());
}

DirectX::SimpleMath::Vector3 GameCamera::ViewportToScreenPoint(const DirectX::SimpleMath::Vector3& point) const
{
	return Vector3::Transform(point, viewport);
}
