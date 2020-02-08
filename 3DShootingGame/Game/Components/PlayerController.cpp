#include "pch.h"
#include "PlayerController.h"
#include <Framework/Components/Transform.h>
#include <Utilities/Input.h>
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Framework/Physics/Rigidbody.h>
#include <Utilities/Math3DUtils.h>
#include "AudioSource.h"
#include "Framework/Context/WindowHandler.h"

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

		if (m_moving && rigid.GetVelocity().LengthSquared() < stopSpeed * stopSpeed)
		{
			rigid.SetVelocity(Vector3::Zero);
			m_moving = false;
			m_movable = true;
		}

		if (m_movable)
		{
			if (Input::GetMouseButton(Input::Buttons::MouseRight))
				m_dragging = false;
			
			auto& transform = rigid.Fetch();

			if (powerTime < 0)
			{
				if (Input::GetMouseButtonDown(Input::Buttons::MouseLeft))
				{
					Input::SetMousePosition(GameContext::Get<WindowHandler>().GetSize() / 2);
					m_beginDrag = Input::GetMousePosition();
					m_dragging = true;
				}
				if (m_dragging)
				{
					m_lastPos = transform.position;
					m_lastRot = transform.rotation;

					m_endDrag = Input::GetMousePosition();
					auto drag = m_endDrag - m_beginDrag;
					auto vec = Vector3::Transform(drag,
						Matrix::CreateRotationX(XM_PIDIV2) *
						Matrix::CreateFromQuaternion(m_camera->GetRotation()) *
						Matrix::CreateRotationY(XM_PI));
					vec.y = 0;
					vec.Normalize();
					vec.y = drag.Length() / (GameContext::Get<WindowHandler>().GetSize().y / 2);
					vec.Normalize();

					if (drag.LengthSquared() > .25f)
						transform.rotation = Math3DUtils::LookAt(Vector3::Zero, vec);
					rigid.Apply();
					if (Input::GetMouseButtonUp(Input::Buttons::MouseLeft))
					{
						m_dragging = false;
						//auto drag = m_endDrag - m_beginDrag;
						//if (drag.LengthSquared() > .25f)
						powerTime = 0;
					}
				}
			}
			else
			{
				if (Input::GetMouseButtonDown(Input::Buttons::MouseLeft))
				{
					m_movable = false;
					m_moving = true;
					powerTime = -1;
					//}
					//if (Input::GetMouseButtonUp(Input::Buttons::MouseRight))
					//{

					rigid.AddForce(Vector3::Transform(Vector3::Forward, transform.rotation) * power * percent);
					rigid.Apply();

					if (gameObject.HasComponent<AudioSource>())
						gameObject.GetComponent<AudioSource>().Play();
				}
				powerTime += float(GameContext::Get<DX::StepTimer>().GetElapsedSeconds()) * speed;
				percent = 1 - std::abs(std::cos(powerTime * 2)) * .9f;
			}
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
	// スプライトバッチ
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	// 球
	m_sphereModel = DirectX::GeometricPrimitive::CreateTetrahedron(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	DX::ThrowIfFailed(device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.ReleaseAndGetAddressOf()));
	// テクスチャ
	DX::ThrowIfFailed(CreateWICTextureFromFile(
		device, context,
		L"Resources/Textures/Play/Control/Launch/Arrow.png", nullptr, m_textureArrow.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateWICTextureFromFile(
		device, context,
		L"Resources/Textures/Play/Control/Launch/Circle.png", nullptr, m_textureCircle.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateWICTextureFromFile(
		device, context,
		L"Resources/Textures/Play/Control/Launch/Base.png", nullptr, m_textureBase.ReleaseAndGetAddressOf()));
}

void PlayerController::Render(GameCamera& camera)
{
	m_camera = &camera;

	auto device = GameContext::Get<DX::DeviceResources>().GetD3DDevice();
	auto context = GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext();
	auto& states = GameContext::Get<DirectX::CommonStates>();
	auto& transform = gameObject.GetComponent<Transform>();

	context->OMSetBlendState(states.Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states.DepthDefault(), 0);

	m_basicEffect->SetWorld(transform.GetMatrix());
	m_basicEffect->SetView(camera.view);
	m_basicEffect->SetProjection(camera.projection);

	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());


	if (lineCountDiv > 0)
	{
		auto acc = Vector3(0, lineGravity, 0);
		auto vel = Vector3::Transform(Vector3::Forward, m_lastRot) * linePowerScale * (power / 1380);
		auto pos = m_lastPos;
		for (int i = 0; i < lineCount * lineCountDiv; i++)
		{
			pos += vel / float(lineCountDiv);
			vel += acc / float(lineCountDiv);
			m_sphereModel->Draw(
				Matrix::CreateRotationX(-XM_PIDIV2) *
				Matrix::CreateScale(Vector3(.5f, 1.5f, .5f)) *
				Matrix::CreateFromQuaternion(Math3DUtils::FromToRotation(Vector3::Up, vel)) *
				Matrix::CreateTranslation(pos),
				camera.view, camera.projection, Color(1, 1, 1, .5f));
				// m_primitiveBatch->Begin();
				// m_primitiveBatch->DrawLine(VertexPositionColor(Vector3::Zero, Colors::Yellow), VertexPositionColor(Vector3::Up, Colors::Yellow));
				// m_primitiveBatch->End();
		}
	}
}

void PlayerController::RenderGui(GameCamera& camera)
{
	if (m_dragging && Vector3::DistanceSquared(m_beginDrag, m_endDrag) > .25f)
	{
		m_spriteBatch->Begin(SpriteSortMode_Deferred, GameContext::Get<CommonStates>().NonPremultiplied());

		auto color = Color(1, 1, 1, .5f);
		m_spriteBatch->Draw(m_textureArrow.Get(), static_cast<Vector2>(Vector3::Lerp(m_beginDrag, m_endDrag, -3/3.f)),nullptr, color,
			std::atan2(m_endDrag.y - m_beginDrag.y, m_endDrag.x - m_beginDrag.x) - XM_PIDIV2, Vector2(32, 32), color);
		m_spriteBatch->Draw(m_textureCircle.Get(), static_cast<Vector2>(Vector3::Lerp(m_beginDrag, m_endDrag, -2/3.f)) - Vector2(16, 16), color);
		m_spriteBatch->Draw(m_textureCircle.Get(), static_cast<Vector2>(Vector3::Lerp(m_beginDrag, m_endDrag, -1/3.f)) - Vector2(16, 16), color);
		m_spriteBatch->Draw(m_textureCircle.Get(), static_cast<Vector2>(Vector3::Lerp(m_beginDrag, m_endDrag, 0/3.f)) - Vector2(16, 16), color);
		m_spriteBatch->Draw(m_textureCircle.Get(), static_cast<Vector2>(Vector3::Lerp(m_beginDrag, m_endDrag, 1/3.f)) - Vector2(16, 16), color);
		m_spriteBatch->Draw(m_textureCircle.Get(), static_cast<Vector2>(Vector3::Lerp(m_beginDrag, m_endDrag, 2/3.f)) - Vector2(16, 16), color);
		m_spriteBatch->Draw(m_textureBase.Get(), static_cast<Vector2>(Vector3::Lerp(m_beginDrag, m_endDrag, 3/3.f)) - Vector2(64, 64), color);

		m_spriteBatch->End();
	}
}

void PlayerController::EditorGui()
{
	ImGui::DragFloat("Power", &power);
	ImGui::DragFloat("Sensitivity", &sensitivity);
	ImGui::DragFloat("Stop Speed", &stopSpeed);
	ImGui::DragFloat("Line Gravity", &lineGravity);
	ImGui::DragInt("Line Count", &lineCount);
	ImGui::DragInt("Line Count Div", &lineCountDiv);
	lineCountDiv = std::max(1, lineCountDiv);
	ImGui::DragFloat("Line Power Scale", &linePowerScale);
	ImGui::DragFloat("Speed", &speed, .01f);
	ImGui::DragFloat("Time", &powerTime, .01f);
	ImGui::DragFloat("Percent", &percent, .01f, 0, 1);
}
