#pragma once

class ApplicationHandler
{
private:
	bool isPlaying = true;

public:
	bool IsPlaying() const;
	void SetPlaying(bool playing);

	// Exit helper
	static void ExitGame();
};

