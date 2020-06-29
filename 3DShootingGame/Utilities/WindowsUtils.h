// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-02 18:33:26 +0900
// Modified: 2019-12-16 07:02:52 +0900

#pragma once

namespace WindowsUtils
{
	// 保存ダイアログ
	bool SaveDialog(const std::string& extension, const std::string& extensionDesc, std::string& result);
	// 開くダイアログ
	bool OpenDialog(const std::string& extension, const std::string& extensionDesc, std::string& result);
	// ディレクトリパス取り出し
	std::string GetDirPath(const std::string& name);
	// ファイルネーム取り出し
	std::string GetFileName(const std::string& name, const std::string& extension);
}



