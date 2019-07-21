#include "pch.h"
#include "ResultScene.h"
#include "SceneCommons.h"
#include <Framework/Scene.h>
#include <Framework/SceneManager.h>
#include <Utilities/Input.h>

using namespace DirectX;

void ResultScene::Build(GameContext& context)
{
	auto& scene = context.GetScene();

	auto obj = scene.AddGameObject();
	auto text = obj->AddComponent<FontObject>(L"Resources/Fonts/logofont.spritefont", L"リザルトシーン");

	class D : public Component
	{
		void Update(GameContext& context)
		{
			if (Input::GetKeyDown(Keyboard::Space))
				context.GetSceneManager().LoadScene(L"TitleScene");
		}
	};
	auto obj2 = scene.AddGameObject();
	obj2->AddComponent<D>();
}
