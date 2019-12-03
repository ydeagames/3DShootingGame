#pragma once

namespace WindowsUtils
{
	// 保存ダイアログ
	bool SaveDialog(const std::string& extension, const std::string& extensionDesc, std::string& result);
	// 開くダイアログ
	bool OpenDialog(const std::string& extension, const std::string& extensionDesc, std::string& result);
	// ファイルネーム取り出し
	std::string GetFileName(const std::string& name, const std::string& extension);
}

