// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-21 03:01:11 +0900
// Modified: 2020-01-05 22:47:30 +0900

#pragma once

// セーブ場所を管理
class SaveHandler
{
private:
	// セーブフォルダ
	std::wstring m_saveDir;

public:
	// saveDir: セーブフォルダへのパス
	SaveHandler(const std::wstring& saveDir)
		: m_saveDir(saveDir)
	{
		CreateDirectoryW(saveDir.c_str(), nullptr);
	}

	// セーブフォルダへのパスを取得
	std::wstring GetDir()
	{
		return m_saveDir;
	}
};



