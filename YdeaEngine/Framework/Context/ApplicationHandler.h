#pragma once

// アプリケーション管理
class ApplicationHandler
{
private:
	// プレイ中(trueのときUpdateが呼ばれる)
	bool isPlaying = true;

public:
	// プレイ中か
	bool IsPlaying() const;
	// プレイ中をセット
	void SetPlaying(bool playing);

	// 終了
	static void ExitGame();
};

