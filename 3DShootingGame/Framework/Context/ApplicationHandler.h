// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-16 06:22:52 +0900
// Modified: 2020-01-05 22:47:30 +0900

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



