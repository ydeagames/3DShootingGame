#include "pch.h"
#include <Framework/ECS/GameContext.h>
#include "ApplicationHandler.h"
#include "SceneManager.h"
#include <Utilities/WindowsUtils.h>

bool ApplicationHandler::IsPlaying() const
{
	return isPlaying;
}

void ApplicationHandler::SetPlaying(bool playing)
{
	if (isPlaying != playing)
	{
		if (playing)
		{
			GameContext::Get<SceneManager>().ForEachScenes([](Scene& scene)
				{
					scene.info.location = WindowsUtils::GetDirPath(scene.info.location)
						+ WindowsUtils::GetFileName(scene.info.location, SceneInfo::SceneExtension)
						+ ".playing." + SceneInfo::SceneExtension;
					scene.Save();
				});
		}
		else
		{
			GameContext::Get<SceneManager>().ForEachScenes([](Scene& scene)
				{
					scene.Load();
					scene.info.location = WindowsUtils::GetDirPath(scene.info.location)
						+ WindowsUtils::GetFileName(WindowsUtils::GetFileName(scene.info.location, SceneInfo::SceneExtension), "playing")
						+ "." + SceneInfo::SceneExtension;
				});
		}

		isPlaying = playing;
	}
}

void ApplicationHandler::ExitGame()
{
	PostQuitMessage(0);
}
