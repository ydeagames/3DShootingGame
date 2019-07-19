#include "pch.h"
#include "PlayScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManager.h>
#include <Framework/DebugTools/DebugCameraWrapper.h>
#include <Framework/DebugTools/GridFloorWrapper.h>
#include <Utilities/Input.h>
#include <Utilities/MathUtils.h>
#include <Framework/Scene.h>
#include <Framework/PhysXManager.h>
#include <Framework/PhysXScene.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PlayScene::Build(GameContext& context)
{
	context.GetCamera().view = Matrix::CreateLookAt(Vector3(0, 5, 10), Vector3::Zero, Vector3::Up);

	struct FPSCamera : Component
	{
		// ‰¡‰ñ“]
		float m_yAngle = 0;
		// c‰ñ“]
		float m_xAngle = 0;
		// ‰¡‰ñ“]
		float m_yAngleLast = m_yAngle;
		// c‰ñ“]
		float m_xAngleLast = m_xAngle;

		void Initialize(GameContext& context)
		{
			Input::SetMouseMode(Mouse::Mode::MODE_RELATIVE);
		}

		void Update(GameContext& context)
		{
			auto move = Input::GetMousePosition();
			Move(float(move.x), float(move.y));

			m_xAngleLast = MathUtils::Lerp(.25f, m_xAngleLast, m_xAngle);
			m_yAngleLast = MathUtils::Lerp(.25f, m_yAngleLast, m_yAngle);

			// ƒrƒ…[s—ñ‚ðŽZo‚·‚é
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
				// ‚xŽ²‚Ì‰ñ“]
				float yAngle = dx * DirectX::XM_PI / 180.0f;
				// ‚wŽ²‚Ì‰ñ“]
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
	auto camera = GameObject::Create();
	//camCtrl->AddComponent<DebugCameraWrapper>();
	camera->AddComponent<FPSCamera>();
	camera->AddComponent<GridFloorWrapper>();
	camera->AddComponent<Plane>();
	context << camera;

	struct Rigidbody : public Component
	{
		physx::PxRigidDynamic* rigid;

		void Initialize(GameContext& context)
		{
			auto& manager = context.GetPhysics();
			auto& scene = context.GetScene().GetPhysics();
			auto size = *gameObject->transform->lossyScale;
			auto radius = (size.x + size.y + size.z) / 3;
			auto geo = physx::PxSphereGeometry(radius);
			auto material = manager.GetPhysics()->createMaterial(1, 1, 1);
			auto shape = manager.GetPhysics()->createShape(geo, *material);
			auto trans = physx::PxTransform(physx::toPhysX(gameObject->transform->position), physx::toPhysX(gameObject->transform->rotation));
			rigid = manager.GetPhysics()->createRigidDynamic(trans);
			rigid->attachShape(*shape);
			scene.CreateObject(*rigid);
		}

		void Update(GameContext& context)
		{
			auto trans = rigid->getGlobalPose();
			gameObject->transform->position = physx::fromPhysX(trans.p);
			gameObject->transform->rotation = physx::fromPhysX(trans.q);
		}

		void AddForce(Vector3 force)
		{
			rigid->addForce(physx::toPhysX(force));
		}

		void SetVelocity(Vector3 velocity)
		{
			rigid->setLinearVelocity(physx::toPhysX(velocity));
		}

		void SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value)
		{
			rigid->setRigidDynamicLockFlag(flag, value);
		}

		Property<std::shared_ptr<Transform>> transform = {
			[&]()
			{
				auto t = std::make_shared<Transform>();
				auto trans = rigid->getGlobalPose();
				t->position = physx::fromPhysX(trans.p);
				t->rotation = physx::fromPhysX(trans.q);
				return t;
			},
			[&](const std::shared_ptr<Transform>& value)
			{
				physx::PxTransform trans;
				trans.p = physx::toPhysX(value->position);
				trans.q = physx::toPhysX(value->rotation);
				rigid->setGlobalPose(trans);
			}
		};
	};
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
				auto bullet = GameObject::Create();
				bullet->AddComponent<GeometricObject>(
					[](GameContext& context) { return GeometricPrimitive::CreateSphere(context.GetDR().GetD3DDeviceContext()); },
					Color(Colors::Yellow)
					);
				bullet->transform->position = *gameObject->transform->position;
				bullet->AddComponent<Rigidbody>();
				context << bullet;
			}
		}
	};
	struct PlayerCamera : public Component
	{
		std::shared_ptr<Transform> player;
		void Update(GameContext& context)
		{
			gameObject->transform->position = Vector3::Lerp(gameObject->transform->position, player->position + Vector3::Up * 2, .25f);
		}
	};
	auto player = GameObject::Create();
	player->AddComponent<GeometricObject>(
		[](GameContext& context) { return GeometricPrimitive::CreateTeapot(context.GetDR().GetD3DDeviceContext()); },
		Color(Colors::Blue)
		);
	player->AddComponent<PlayerBehaviour>();
	player->AddComponent<Rigidbody>();
	context << player;

	auto playerCamera = camera->AddComponent<PlayerCamera>();
	playerCamera->player = player->transform;
}
