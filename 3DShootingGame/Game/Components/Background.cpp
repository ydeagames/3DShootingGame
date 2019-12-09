#include "pch.h"
#include "Background.h"
#include <Common/DeviceResources.h>
#include <Framework/ECS/GameContext.h>
#include <Utilities/WindowsUtils.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Background::RenderStart()
{
	auto& dr = GameContext::Get<DX::DeviceResources>();
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

	std::vector<GeometricPrimitive::VertexType> vertices = {
		{ Vector3(-0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 0.0f) },
		{ Vector3(+0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 0.0f) },
		{ Vector3(+0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 1.0f) },
		{ Vector3(-0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 1.0f) },
	};
	std::vector<uint16_t> indices = {
		0, 1, 2, 0, 2, 3,
	};
	m_plane = GeometricPrimitive::CreateCustom(dr.GetD3DDeviceContext(), vertices, indices);

	std::wstring name = cv.from_bytes(texture);
	if (FAILED(CreateWICTextureFromFile(
		dr.GetD3DDevice(), dr.GetD3DDeviceContext(),
		name.c_str(), nullptr, m_texture.ReleaseAndGetAddressOf())))
		m_texture = nullptr;
}

void Background::Render(GameCamera& camera)
{
	if (m_texture)
		m_plane->Draw(Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity, Colors::White, m_texture.Get());
}

void Background::EditorGui()
{
	std::string tmpname = texture;
	tmpname.resize(128);
	ImGui::InputText("Texture##Background", &tmpname[0], tmpname.size());
	auto tmpname2 = std::string(tmpname.c_str());
	if (texture != tmpname2)
	{
		texture = tmpname2;
		RenderStart();
	}
}
