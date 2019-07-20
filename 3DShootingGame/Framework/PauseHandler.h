#pragma once

class GameContext;

class PauseHandler
{
	bool paused = false;

public:
	void SetPaused(GameContext& context, bool pause);
};

