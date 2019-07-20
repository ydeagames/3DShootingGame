#include "pch.h"
#include "ResultScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManager.h>
#include <Utilities/Input.h>

using namespace DirectX;

void ResultScene::Build(GameContext& context)
{
	auto obj = GameObject::Create();
	auto text = obj->AddComponent<FontObject>(L"Resources/Fonts/logofont.spritefont", L"リザルトシーン");
	context << obj;

	class D : public Component
	{
		void Update(GameContext& context)
		{
			if (Input::GetKeyDown(Keyboard::Space))
				context.GetSceneManager().LoadScene(L"TitleScene");
		}
	};
	auto obj2 = GameObject::Create();
	obj2->AddComponent<D>();
	context << obj2;
}
