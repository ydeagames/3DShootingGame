#include "pch.h"
#include "PlayerController.h"
#include <Framework/Components/Transform.h>
#include <Utilities/Input.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Framework/Physics/Rigidbody.h>
#include <Utilities/Math3DUtils.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PlayerController::Start()
{
	auto& rigid = gameObject.GetComponent<Rigidbody>();
	if (auto dynamic = rigid.rigid->is<physx::PxRigidDynamic>())
	{
		dynamic->setAngularDamping(2);
	}
}

void PlayerController::Update()
{
	if (m_camera)
	{
		auto& rigid = gameObject.GetComponent<Rigidbody>();
		auto& transform = rigid.Fetch();

		if (Input::GetMouseButtonDown(Input::Buttons::MouseLeft))
		{
			m_beginDrag = Input::GetMousePosition();
		}
		if (Input::GetMouseButton(Input::Buttons::MouseLeft))
		{
			m_endDrag = Input::GetMousePosition();

			auto getpos = [&](Vector3 pos)
			{
				float dist;
				auto ray = m_camera->ScreenPointToRay(pos);
				Plane plane(transform.position, Vector3::Up);
				if (ray.Intersects(plane, dist))
				{
					return ray.position + ray.direction * dist;
				}
				return Vector3::Zero;
			};

			auto begin = getpos(m_beginDrag);
			auto end = getpos(m_endDrag);

			if (m_beginDrag != m_endDrag)
				transform.rotation = Math3DUtils::LookAt(end, begin + Vector3::Up);
			rigid.Apply();
		}
		if (Input::GetMouseButtonUp(Input::Buttons::MouseLeft))
		{
			rigid.AddForce(Vector3::Transform(Vector3::Forward, transform.rotation) * power);
			rigid.Apply();
		}
	}
}

void PlayerController::RenderStart()
{
	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();

	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// 頂点カラー(有効)
	m_basicEffect->SetVertexColorEnabled(true);
	// プリミティブオブジェクト生成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	DX::ThrowIfFailed(device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.ReleaseAndGetAddressOf()));
}

void PlayerController::Render(GameCamera& camera)
{
	m_camera = &camera;

	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();
	auto& states = GameContext::Get<DirectX::CommonStates>();

	context->OMSetBlendState(states.Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states.DepthDefault(), 0);

	m_basicEffect->SetWorld(gameObject.GetComponent<Transform>().GetMatrix());
	m_basicEffect->SetView(camera.view);
	m_basicEffect->SetProjection(camera.projection);

	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());

	auto& transform = gameObject.GetComponent<Transform>();
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawLine(VertexPositionColor(Vector3::Zero, Colors::Yellow), VertexPositionColor(Vector3::Forward, Colors::Yellow));
	m_primitiveBatch->End();
}

void PlayerController::EditorGui()
{
	ImGui::DragFloat("Power", &power);
	ImGui::DragFloat("Sensitivity", &sensitivity);
}
