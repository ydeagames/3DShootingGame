#include "pch.h"
#include "BuildSettings.h"
#include <Framework/SceneManager.h>
#include "BaseScene.h"
#include "LogoScene.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "PlayScenePause.h"
#include "ResultScene.h"

const std::wstring BuildSettings::GAME_TITLE = L"カタトウス";
const std::wstring BuildSettings::GAME_TITLE_CLASS = L"KatatousWindowClass";

void BuildSettings::Build(GameContext& context)
{
	context.GetSceneManager().Register<BaseScene>();
	context.GetSceneManager().Register<LogoScene>();
	context.GetSceneManager().Register<TitleScene>();
	context.GetSceneManager().Register<PlayScene>();
	context.GetSceneManager().Register<PlayScenePause>();
	context.GetSceneManager().Register<ResultScene>();

	//context.GetSceneManager().LoadScene(L"BaseScene");
	context.GetSceneManager().LoadScene(L"TitleScene");
	//context.GetSceneManager().LoadScene(L"PlayScene");
}
