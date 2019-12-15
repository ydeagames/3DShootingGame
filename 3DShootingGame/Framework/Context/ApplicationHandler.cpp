#include "pch.h"
#include <Framework/ECS/GameContext.h>
#include "ApplicationHandler.h"
#include "SceneManager.h"

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
			GameContext::Get<SceneManager>().ForEachScenes([](Scene& scene) { scene.Save(); });
		}
		else
		{
			GameContext::Get<SceneManager>().ForEachScenes([](Scene& scene) { scene.Load(); });
		}

		isPlaying = playing;
	}
}

void ApplicationHandler::ExitGame()
{
	PostQuitMessage(0);
}
