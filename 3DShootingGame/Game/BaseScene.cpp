#include "pch.h"
#include "BaseScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManager.h>
#include <Framework/DebugTools/DebugCameraWrapper.h>
#include <Framework/DebugTools/GridFloorWrapper.h>
#include <Utilities/Input.h>
#include <Utilities/picojson.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// ê∂ê¨
void BaseScene::Build(GameContext& context)
{
	/*
	context.GetCamera().view = Matrix::CreateLookAt(Vector3(0, 5, 10), Vector3::Zero, Vector3::Up);

	auto bit = std::make_shared<GeometricObject>(
			[](GameContext& context) { return GeometricPrimitive::CreateTeapot(context.GetDR().GetD3DDeviceContext()); },
			Color(Colors::Blue)
		);
	context << bit;
	*/

	context.GetSceneManager().LoadScene("LogoScene");
}
