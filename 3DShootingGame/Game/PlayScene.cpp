#include "pch.h"
#include "PlayScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManagement/Scene.h>
#include <Framework/SceneManagement/SceneManager.h>
#include <Framework/DebugTools/DebugCameraWrapper.h>
#include <Framework/DebugTools/GridFloorWrapper.h>
#include <Utilities/Input.h>
#include <Utilities/MathUtils.h>
#include <Utilities/Random.h>
#include <Framework/PhysX/PhysXManager.h>
#include <Framework/PhysX/PhysXScene.h>
#include <Framework/ImGui/ImGuiManager.h>
#include <Framework/PauseHandler.h>
#include <Framework/Physics/Collider.h>
#include <Framework/Physics/Rigidbody.h>
#include <Framework/WindowHandler.h>
#include <Utilities/BinaryFile.h>
#include "InfinityGridFloor.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace
{
	ObjectHolder<float> targetScale = ObjectHolder<float>::Create(.00175f);
	ObjectHolder<float> targetY = ObjectHolder<float>::Create(.5f);
}

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
	struct Menu : public Component
	{
		std::shared_ptr<ISceneBuilder> m_window;
		float last;

		void Initialize(GameContext& context)
		{
			last = float(context.GetTimer().GetTotalSeconds());
		}

		void Render(GameContext& context)
		{
			auto& scene = context.GetScene();

			ImGui::SetNextWindowPos(ImVec2(10, 10));
			ImGui::Begin(u8"パラメータ", nullptr);
			ImGui::SliderFloat(u8"的サイズ", targetScale.GetWeakPtr().lock().get(), 0, .003f, u8"%.6f");
			ImGui::SliderFloat(u8"Y", targetY.GetWeakPtr().lock().get(), 0, 6, u8"%.2f");
			if (ImGui::Button(u8"リセットいがぐり"))
			{
				auto find = scene.FindAll(L"Bullet");
				for (auto& f : find)
					Destroy(**f);
			}
			if (ImGui::Button(u8"リセットターゲット"))
			{
				auto find = scene.FindAll(L"Target");
				for (auto& f : find)
					Destroy(**f);
			}
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(300, 10));
			ImGui::Begin(u8"スコア", nullptr);
			ImGui::Text(u8"スコア: 00000032435 (未実装)");
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(10, 500));
			ImGui::Begin(u8"タイム", nullptr);
			ImGui::Text(u8"タイム: %.2f秒", float(context.GetTimer().GetTotalSeconds()) - last);
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(10, 300));
			ImGui::Begin(u8"目標", nullptr);
			ImGui::Text(u8"一番てっぺんに登れ (仮)");
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(10, 700));
			ImGui::Begin(u8"操作説明", nullptr);
			ImGui::Text(u8"1. 右クリック長押しで力をためる");
			ImGui::Text(u8"2. 右クリックを離してスリングジャーンプ！");
			ImGui::End();
		}

		void Update(GameContext& context)
		{
			if (Input::GetKeyDown(Keyboard::Keys::Escape))
				context.GetPauseHandler().SetPaused(context, false);
		}

		void Finalize(GameContext& context)
		{
			Destroy(*m_window);
		}
	};
	auto menu = scene.AddGameObject();
	menu->AddComponent<PauseBehaviour>();
	menu->AddComponent<Menu>();

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
			if (!Input::GetMouseButton(Input::Buttons::MouseRight))
				if (Input::GetMouseMode() == DirectX::Mouse::Mode::MODE_RELATIVE)
				{
					auto move = Input::GetMousePosition();
					Move(float(move.x), float(move.y));
				}

			m_xAngle = MathUtils::Clamp(m_xAngle, -XM_PIDIV2, XM_PIDIV2);

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
				float yAngle = XMConvertToRadians(dx);
				// Ｘ軸の回転
				float xAngle = XMConvertToRadians(dy);

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
			auto& scene = context.GetPhysicsScene();
			auto geo = physx::PxPlane(physx::toPhysX(Vector3::Zero), physx::toPhysX(Vector3::Up));
			auto material = manager.CreateMaterial(PhysicsMaterials::Wood);
			auto rigid = physx::PxCreatePlane(*manager.GetPhysics(), geo, *material);
			scene.CreateObject(*rigid);
		}
	};
	struct Terrain : public Component
	{
		std::unique_ptr<Model> m_model;

		void Initialize(GameContext& context)
		{
			auto& manager = context.GetPhysics();
			auto& scene = context.GetPhysicsScene();

			Transform localTransform;
			auto trans = physx::PxTransform(physx::toPhysX(gameObject->transform->position), physx::toPhysX(gameObject->transform->rotation));
			auto rigid = manager.GetPhysics()->createRigidStatic(trans);
			auto material = manager.CreateMaterial(PhysicsMaterials::Wood);

			int numRows = 33, numCols = 33;

			BinaryFile HFData = BinaryFile::LoadFile(L"Resources/Textures/Play/Terrain.raw");
			physx::PxHeightFieldSample* samples = new physx::PxHeightFieldSample[numRows * numCols];
			uint16_t* Int16HFData = reinterpret_cast<uint16_t*>(HFData.GetData());
			for (int i = 0; i < numRows * numCols; i++)
				samples[i].height = Int16HFData[i];

			physx::PxHeightFieldDesc hfDesc;
			hfDesc.format = physx::PxHeightFieldFormat::eS16_TM;
			hfDesc.nbColumns = numCols;
			hfDesc.nbRows = numRows;
			hfDesc.samples.data = samples;
			hfDesc.samples.stride = sizeof(physx::PxHeightFieldSample);

			physx::PxHeightField* hf = manager.GetCooking()->createHeightField(hfDesc,
				manager.GetPhysics()->getPhysicsInsertionCallback());

			auto geo = physx::PxHeightFieldGeometry(hf, physx::PxMeshGeometryFlags(), 1.f / 256, 1.f * 4, 1.f * 4);
			auto shape = manager.GetPhysics()->createShape(geo, *material);
			shape->setLocalPose(physx::PxTransform(physx::toPhysX(localTransform.position), physx::toPhysX(localTransform.rotation)));
			rigid->attachShape(*shape);
			scene.CreateObject(*rigid);

			m_model = Model::CreateFromCMO(context.GetDR().GetD3DDevice(), L"Resources/Models/Terrain.cmo", context.GetEffectFactory());
		}

		void Render(GameContext& context)
		{
			m_model->Draw(context.GetDR().GetD3DDeviceContext(), context.GetStates(), gameObject->transform->GetMatrix() * Matrix::CreateRotationY(XM_PIDIV2), context.GetCamera().view, context.GetCamera().projection);
		}
	};
	struct InfinityGridFloorWrapper : public Component
	{
		// グリッド床
		std::unique_ptr<InfinityGridFloor> m_pGridFloor;
		// 生成
		void Initialize(GameContext& context)
		{
			// グリッド床作成
			m_pGridFloor = std::make_unique<InfinityGridFloor>(context, 2.f, Vector2(200, 200));
		}
		// 描画
		void Render(GameContext& context)
		{
			m_pGridFloor->draw(context, Colors::White);
		}
	};
	auto terrain = scene.AddGameObject(L"Terrain");
	//terrain->AddComponent<GridFloorWrapper>();
	terrain->AddComponent<InfinityGridFloorWrapper>();
	terrain->AddComponent<Plane>();
	terrain->AddComponent<Terrain>();

	auto camera = scene.AddGameObject();
	//camCtrl->AddComponent<DebugCameraWrapper>();
	camera->AddComponent<FPSCamera>();

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
			m_skydoom->CreateInputLayout(m_pBasicEffect.get(), m_pInputLayout.ReleaseAndGetAddressOf());
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

	struct ModelRenderer : public Component
	{
		Model* m_model;
		ModelRenderer(Model* model) : m_model(model) {}

		void Render(GameContext& context)
		{
			m_model->Draw(context.GetDR().GetD3DDeviceContext(), context.GetStates(), gameObject->transform->GetMatrix(), context.GetCamera().view, context.GetCamera().projection);
		}
	};

	struct PlayerBehaviour : public Component
	{
		std::unique_ptr<Model> m_model;
		bool charging = false;
		Vector3 chargingOrigin = Vector3::Zero;

		void Initialize(GameContext& context)
		{
			m_model = Model::CreateFromCMO(context.GetDR().GetD3DDevice(), L"Resources/Models/igaguri.cmo", context.GetEffectFactory());
		}

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
			rigid->AddForce(force * 30.f);

			if (Input::GetMouseButtonDown(Input::Buttons::MouseLeft))
			{
				auto& scene = context.GetScene();
				auto bullet = scene.AddGameObject(L"Bullet");
				auto bulletModel = scene.AddGameObject(L"BulletModel");
				bulletModel->AddComponent<ModelRenderer>(m_model.get());
				bulletModel->transform->parent = *bullet->transform;
				bulletModel->transform->localScale = Vector3(.0175f);
				//bullet->AddComponent<GeometricObject>(
				//	[](GameContext& context) { return GeometricPrimitive::CreateSphere(context.GetDR().GetD3DDeviceContext()); },
				//	Color(Colors::Yellow)
				//	);
				auto ray = context.GetCamera().ViewportPointToRay(Vector3::Zero);
				ray.direction.Normalize();
				bullet->transform->position = gameObject->transform->position + ray.direction * 5;
				auto rigidbody = bullet->AddComponent<Rigidbody>();
				rigidbody->Add(std::make_shared<SphereCollider>());
				rigidbody->SetVelocity(ray.direction * 50);
			}

			if (Input::GetMouseButtonDown(Input::Buttons::MouseRight))
			{
				charging = true;
				chargingOrigin = Vector3::Zero;
			}
			if (Input::GetMouseButton(Input::Buttons::MouseRight))
			{
				chargingOrigin += Input::GetMousePosition();
			}
			if (Input::GetMouseButtonUp(Input::Buttons::MouseRight))
			{
				auto length = MathUtils::Clamp(0.f, 1.f, (chargingOrigin.y - Input::GetMousePosition().y) * .5f / 50.f);
				//length = 1 - (1 - length) * (1 - length);
				if (length > .1f)
					length += .2f;
				else
					length = 0;
				length *= 50.f;
				auto ray = context.GetCamera().ViewportPointToRay(Vector3::Zero);
				ray.direction.Normalize();
				rigid->SetVelocity(ray.direction * length);
				charging = false;
			}
		}

		void Render(GameContext& context)
		{
			static auto model = GeometricPrimitive::CreateCone(context.GetDR().GetD3DDeviceContext());
			if (charging)
			{
				auto length = MathUtils::Clamp(0.f, 1.f, (chargingOrigin.y - Input::GetMousePosition().y) * .5f / 50.f);
				if (length > .1f)
					length += .2f;
				else
					length = 0;
				length *= 50.f;
				auto ray = context.GetCamera().ViewportPointToRay(Vector3::Zero);
				auto dir = ray.direction;
				dir.Normalize();
				auto pos = ray.position + dir;
				auto world = Matrix::Identity
					* Matrix::CreateScale(1, 1 + length, 1)
					* Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3::UnitX, -XM_PIDIV2))
					* Matrix::CreateTranslation(Vector3::Forward * 3 + Vector3::Down * 2)
					* Matrix::CreateFromQuaternion(context.GetCamera().GetRotation())
					* Matrix::CreateTranslation(pos)
					;
				model->Draw(world, context.GetCamera().view, context.GetCamera().projection);
				ImGui::Begin("Debug1");
				ImGui::InputFloat3("Pos", &pos.x);
				ImGui::End();
			}
		}
	};
	struct PlayerCamera : public Component
	{
		ObjectField<Transform> player;
		void Update(GameContext& context)
		{
			gameObject->transform->position = Vector3::Lerp(gameObject->transform->position, player->position, .25f);
		}
	};

	{
		auto player = scene.AddGameObject();
		//player->AddComponent<GeometricObject>(
		//	[](GameContext& context) { return GeometricPrimitive::CreateTeapot(context.GetDR().GetD3DDeviceContext()); },
		//	Color(Colors::Blue)
		//	);
		player->AddComponent<PlayerBehaviour>();
		auto rigidbody = player->AddComponent<Rigidbody>();
		auto col = rigidbody->Add(std::make_shared<BoxCollider>());
		col->material = PhysicsMaterials::Wood;

		auto playerCamera = camera->AddComponent<PlayerCamera>();
		playerCamera->player = player->transform;
	}

	for (int i = 0; i < 100; i++)
	{
		auto box = scene.AddGameObject(L"Cube");
		box->transform->localScale = Vector3(float(Random::Range(1, 10)), float(Random::Range(1, 5)), float(Random::Range(1, 10)));
		box->transform->localPosition = Vector3(float(Random::Range(-50, 50)), float(Random::Range(3, 50)), float(Random::Range(-50, 50)));
		box->AddComponent<GeometricObject>(
			[](GameContext& context) { return GeometricPrimitive::CreateCube(context.GetDR().GetD3DDeviceContext()); },
			Color(Vector4(Random::Range(0.f, 1.f), Random::Range(0.f, 1.f), Random::Range(0.f, 1.f), 1.f))
			);
		auto rigid = box->AddComponent<Rigidbody>();
		rigid->SetStatic(true);
		auto col = rigid->Add(std::make_shared<BoxCollider>());
	}

	struct TargetGenerator : public Component
	{
		float time = 0;
		float span = 10;

		std::unique_ptr<Model> m_model;

		void Initialize(GameContext& context)
		{
			m_model = Model::CreateFromCMO(context.GetDR().GetD3DDevice(), L"Resources/Models/target.cmo", context.GetEffectFactory());
		}

		void Update(GameContext& context)
		{
			time += float(context.GetTimer().GetElapsedSeconds());
			if (time > span)
			{
				auto& scene = context.GetScene();
				auto target = scene.AddGameObject(L"Target");
				auto targetModel = scene.AddGameObject();
				targetModel->AddComponent<ModelRenderer>(m_model.get());
				targetModel->transform->parent = *target->transform;
				targetModel->transform->localScale = Vector3(**targetScale);
				struct A : public Component
				{
					void Render(GameContext& context)
					{
						gameObject->transform->localScale = Vector3(**targetScale);
					}
				};
				targetModel->AddComponent<A>();
				auto rigidbody = target->AddComponent<Rigidbody>();
				{
					auto col = rigidbody->Add(std::make_shared<BoxCollider>());
					col->localTransform.localScale = Vector3(.7f, .7f, .1f);
					col->localTransform.localPosition = Vector3(0.f, 4.5f, 0.f);
				}
				{
					auto col = rigidbody->Add(std::make_shared<BoxCollider>());
					col->localTransform.localScale = Vector3(.5f, .1f, .5f);
					col->localTransform.localPosition = Vector3(0.f, 0.f, 0.f);
				}
				target->transform->position = Vector3(Random::Range(-25.f, 25.f), .5f, Random::Range(-25.f, 25.f));
				target->transform->localScale = Vector3(4);
				time = 0;
			}
		}
	};
	auto targen = scene.AddGameObject();
	targen->AddComponent<TargetGenerator>();

	struct CenterUI : public Component
	{
		std::unique_ptr<GeometricPrimitive> m_plane;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

		Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
		float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		void Initialize(GameContext& context)
		{
			auto device = context.GetDR().GetD3DDevice();

			std::vector<GeometricPrimitive::VertexType> vertices = {
				{ Vector3(-0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 0.0f) },
				{ Vector3(+0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 0.0f) },
				{ Vector3(+0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 1.0f) },
				{ Vector3(-0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 1.0f) },
			};
			std::vector<uint16_t> indices = {
				0, 1, 2, 0, 2, 3,
			};
			m_plane = GeometricPrimitive::CreateCustom(context.GetDR().GetD3DDeviceContext(), vertices, indices);

			DX::ThrowIfFailed(CreateWICTextureFromFile(
				context.GetDR().GetD3DDevice(), context.GetDR().GetD3DDeviceContext(),
				L"Resources/Textures/UI/Center.png", nullptr, m_texture.ReleaseAndGetAddressOf()));

			D3D11_BLEND_DESC BlendDesc = {};
			BlendDesc.AlphaToCoverageEnable = FALSE;
			BlendDesc.IndependentBlendEnable = FALSE;
			BlendDesc.RenderTarget[0].BlendEnable = TRUE;
			BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
			BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
			BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			device->CreateBlendState(&BlendDesc, m_blendState.ReleaseAndGetAddressOf());
		}

		void Render(GameContext& context)
		{
			auto ctx = context.GetDR().GetD3DDeviceContext();

			auto size = context.Get<WindowHandler>().GetSize();
			m_plane->Draw(SimpleMath::Matrix::CreateScale(Vector3(size.x, size.y, 1.f)).Invert() * Matrix::CreateScale(64), Matrix::Identity, Matrix::Identity, Colors::White, m_texture.Get(), false, [&]() {
				ctx->OMSetBlendState(m_blendState.Get(), m_blendFactor, 0xFFFFFFFF);
				});
		}
	};
	auto background = scene.AddGameObject();
	background->AddComponent<CenterUI>();
}
