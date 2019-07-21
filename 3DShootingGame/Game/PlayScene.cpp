#include "pch.h"
#include "PlayScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManager.h>
#include <Framework/DebugTools/DebugCameraWrapper.h>
#include <Framework/DebugTools/GridFloorWrapper.h>
#include <Utilities/Input.h>
#include <Utilities/MathUtils.h>
#include <Utilities/Random.h>
#include <Framework/Scene.h>
#include <Framework/PhysX/PhysXManager.h>
#include <Framework/PhysX/PhysXScene.h>
#include <Framework/PauseHandler.h>
#include <Framework/Collider.h>
#include <Framework/Rigidbody.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PlayScene::Build(GameContext& context)
{
	auto& scene = context.GetScene();
	scene.mouseMode = DirectX::Mouse::Mode::MODE_RELATIVE;

	context.GetCamera().view = Matrix::CreateLookAt(Vector3(0, 5, 10), Vector3::Zero, Vector3::Up);

	struct PauseBehaviour : public Component
	{
		void Update(GameContext& context)
		{
			if (Input::GetKeyDown(Keyboard::Keys::Escape))
				context.GetPauseHandler().SetPaused(context, true);
		}
	};
	auto pausedirector = scene.AddGameObject();
	pausedirector->AddComponent<PauseBehaviour>();

	struct FPSCamera : Component
	{
		// 横回転
		float m_yAngle = 0;
		// 縦回転
		float m_xAngle = 0;
		// 横回転
		float m_yAngleLast = m_yAngle;
		// 縦回転
		float m_xAngleLast = m_xAngle;

		void Initialize(GameContext& context)
		{
			Input::SetMouseMode(Mouse::Mode::MODE_RELATIVE);
		}

		void Update(GameContext& context)
		{
			if (Input::GetMouseMode() == DirectX::Mouse::Mode::MODE_RELATIVE)
			{
				auto move = Input::GetMousePosition();
				Move(float(move.x), float(move.y));
			}

			m_xAngleLast = MathUtils::Lerp(.25f, m_xAngleLast, m_xAngle);
			m_yAngleLast = MathUtils::Lerp(.25f, m_yAngleLast, m_yAngle);

			// ビュー行列を算出する
			Quaternion rotX = Quaternion::CreateFromAxisAngle(Vector3::UnitX, -m_xAngleLast);
			Quaternion rotY = Quaternion::CreateFromAxisAngle(Vector3::UnitY, -m_yAngleLast);
			Quaternion rot = -rotX * rotY;

			Matrix rt = Matrix::CreateFromQuaternion(rot);

			Vector3 eye(0.0f, 0.0f, 0.0f);
			Vector3 target(0.0f, 0.0f, -1.0f);
			Vector3 up(0.0f, 1.0f, 0.0f);

			target = Vector3::Transform(target, rt);
			up = Vector3::Transform(up, rt);

			auto pos = gameObject->transform->position;
			auto view = Matrix::CreateLookAt(eye + pos, target + pos, up);
			context.GetCamera().view = view;
		}

		void Move(float dx, float dy)
		{
			if (dx != 0.0f || dy != 0.0f)
			{
				// Ｙ軸の回転
				float yAngle = dx * DirectX::XM_PI / 180.0f;
				// Ｘ軸の回転
				float xAngle = dy * DirectX::XM_PI / 180.0f;

				m_xAngle += xAngle;
				m_yAngle += yAngle;
			}
		}
	};
	struct Plane : public Component
	{
		void Initialize(GameContext& context)
		{
			auto& manager = context.GetPhysics();
			auto& scene = context.GetScene().GetPhysics();
			auto geo = physx::PxPlane(physx::toPhysX(Vector3::Zero), physx::toPhysX(Vector3::Up));
			auto material = manager.GetPhysics()->createMaterial(1, 1, 1);
			auto rigid = physx::PxCreatePlane(*manager.GetPhysics(), geo, *material);
			scene.CreateObject(*rigid);
		}
	};
	auto camera = scene.AddGameObject();
	//camCtrl->AddComponent<DebugCameraWrapper>();
	camera->AddComponent<FPSCamera>();
	camera->AddComponent<GridFloorWrapper>();
	camera->AddComponent<Plane>();

	struct Skydoom : public Component
	{
		std::unique_ptr<GeometricPrimitive> m_skydoom;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
		std::unique_ptr<DirectX::BasicEffect> m_pBasicEffect;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

		void Initialize(GameContext& context)
		{
			m_skydoom = GeometricPrimitive::CreateGeoSphere(context.GetDR().GetD3DDeviceContext(), 1.f, 3U, false);

			DX::ThrowIfFailed(CreateWICTextureFromFile(
				context.GetDR().GetD3DDevice(), context.GetDR().GetD3DDeviceContext(),
				L"Resources/Textures/Play/Skydoom.jpg", nullptr, m_texture.ReleaseAndGetAddressOf()));

			// ポリゴン用エフェクト作成
			m_pBasicEffect = std::make_unique<BasicEffect>(context.GetDR().GetD3DDevice());
			m_pBasicEffect->SetTextureEnabled(true);

			// ライト有効
			m_pBasicEffect->SetLightingEnabled(true);
			// 環境光の色を設定
			m_pBasicEffect->SetAmbientLightColor(SimpleMath::Vector3(0.2f, 0.2f, 0.2f));
			// 拡散反射光の素材色を設定
			m_pBasicEffect->SetDiffuseColor(SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

			// シェーダー取得
			m_skydoom->CreateInputLayout(m_pBasicEffect.get(), m_pInputLayout.GetAddressOf());
		}

		void Render(GameContext& context)
		{
			auto ctx = context.GetDR().GetD3DDeviceContext();
			// ワールド行列設定
			m_pBasicEffect->SetWorld(gameObject->transform->GetMatrix());
			// ビュー行列設定
			m_pBasicEffect->SetView(context.GetCamera().view);
			// プロジェクション行列設定
			m_pBasicEffect->SetProjection(context.GetCamera().projection);
			// エフェクトの設定
			m_pBasicEffect->Apply(ctx);
			// 深度ステンシルステートの設定
			ctx->OMSetDepthStencilState(context.GetStates().DepthDefault(), 0);
			// ブレンドステートの設定
			ctx->OMSetBlendState(context.GetStates().AlphaBlend(), nullptr, 0xffffffff);
			// ラスタライザステートを設定
			ctx->RSSetState(context.GetStates().CullClockwise());
			// 入力レイアウトの設定
			ctx->IASetInputLayout(m_pInputLayout.Get());

			m_pBasicEffect->SetTexture(m_texture.Get());

			m_skydoom->Draw(m_pBasicEffect.get(), m_pInputLayout.Get());
			//m_skydoom->Draw(gameObject->transform->GetMatrix(), context.GetCamera().view, context.GetCamera().projection, Colors::White, m_texture.Get(), false);
		}
	};
	auto skydoom = scene.AddGameObject();
	skydoom->transform->localScale = Vector3(1000);
	skydoom->AddComponent<Skydoom>();

	struct PlayerBehaviour : public Component
	{
		void Update(GameContext& context)
		{
			auto input = Vector3::Zero;
			if (Input::GetKey(Keyboard::Keys::A) || Input::GetKey(Keyboard::Keys::Left))
				input.x -= 1;
			if (Input::GetKey(Keyboard::Keys::D) || Input::GetKey(Keyboard::Keys::Right))
				input.x += 1;
			if (Input::GetKey(Keyboard::Keys::W) || Input::GetKey(Keyboard::Keys::Up))
				input.z -= 1;
			if (Input::GetKey(Keyboard::Keys::S) || Input::GetKey(Keyboard::Keys::Down))
				input.z += 1;

			Vector3 s_, t_;
			Quaternion rotation;
			context.GetCamera().view.Decompose(s_, rotation, t_);
			rotation.Inverse(rotation);
			input = Vector3::Transform(input, rotation);
			input.y = 0;
			input.Normalize();

			auto flat = Vector3::Transform(Vector3::Forward, rotation);
			flat.y = 0;
			auto flatRotation = Quaternion::CreateFromRotationMatrix(Matrix::CreateWorld(Vector3::Zero, flat, Vector3::Up));
			gameObject->transform->localRotation = flatRotation;

			auto force = input;
			gameObject->transform->localPosition += force * .1f;

			auto rigid = gameObject->GetComponent<Rigidbody>();
			rigid->SetVelocity(force * 10.f);

			if (Input::GetMouseButtonDown(Input::Buttons::MouseLeft))
			{
				auto& scene = context.GetScene();
				auto bullet = scene.AddGameObject();
				bullet->AddComponent<GeometricObject>(
					[](GameContext& context) { return GeometricPrimitive::CreateSphere(context.GetDR().GetD3DDeviceContext()); },
					Color(Colors::Yellow)
					);
				auto ray = context.GetCamera().ViewportPointToRay(Vector3::Zero);
				ray.direction.Normalize();
				bullet->transform->position = *gameObject->transform->position + ray.direction * 5;
				auto rigidbody = bullet->AddComponent<Rigidbody>();
				rigidbody->Add(std::make_shared<SphereCollider>());
				rigidbody->SetVelocity(ray.direction * 50);
			}
		}
	};
	struct PlayerCamera : public Component
	{
		ObjectField<Transform> player;
		void Update(GameContext& context)
		{
			gameObject->transform->position = Vector3::Lerp(gameObject->transform->position, player->position + Vector3::Up * 2, .25f);
		}
	};
	auto player = scene.AddGameObject();
	player->AddComponent<GeometricObject>(
		[](GameContext& context) { return GeometricPrimitive::CreateTeapot(context.GetDR().GetD3DDeviceContext()); },
		Color(Colors::Blue)
		);
	player->AddComponent<PlayerBehaviour>();
	auto rigidbody = player->AddComponent<Rigidbody>();
	rigidbody->Add(std::make_shared<BoxCollider>());

	auto playerCamera = camera->AddComponent<PlayerCamera>();
	playerCamera->player = player->transform;

	{
		auto box = scene.AddGameObject();
		box->transform->localScale = Vector3(4);
		box->transform->localPosition = Vector3(8, 0, 3);
		auto rigid = box->AddComponent<Rigidbody>();
		rigid->SetStatic(true);
		auto col = rigid->Add(std::make_shared<BoxCollider>());
		col->localTransform.localScale = Vector3(4);
	}
}
