#include "pch.h"
#include "LogoScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManagement/Scene.h>
#include <Framework/SceneManagement/SceneManager.h>

void LogoScene::Build(GameContext& context)
{
	auto& scene = context.GetScene();

	auto obj = scene.AddGameObject();
	auto text = obj->AddComponent<FontObject>(L"Resources/Fonts/logofont.spritefont", L"ÉçÉSÉVÅ[Éì");

	class A : public Component
	{
		float lastTime;
		ObjectField<FontObject> m_text;

	public:
		A(const ObjectField<FontObject>& text)
			: m_text(text) {}

		void Initialize(GameContext& context)
		{
			lastTime = float(context.GetTimer().GetTotalSeconds());
		}

		void Update(GameContext& context)
		{
			float time = float(context.GetTimer().GetTotalSeconds()) - lastTime;

			std::wostringstream sb;
			sb << L"ÉçÉSÉVÅ[Éì: " << std::setprecision(2) << time << L"ïb";

			m_text->SetText(sb.str());

			if (time > 3.f)
				context.GetSceneManager().LoadScene(L"TitleScene");
		}
	};
	auto obj1 = scene.AddGameObject();
	auto cmp = obj1->AddComponent<A>(text);
}
