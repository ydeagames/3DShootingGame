#include "pch.h"
#include "WindowsUtils.h"
#include "StringCast.h"

namespace WindowsUtils
{
	// 保存ダイアログ
	bool SaveDialog(const std::string& extension, const std::string& extensionDesc, std::string& result)
	{
		// フィルター文字列定義
		std::wstringstream filter;
		std::wstring wextension = string_cast<std::wstring>(extension);
		filter << string_cast<std::wstring>(extensionDesc) << L" (*." << wextension << L")" << std::ends;
		filter << L"*." << wextension << std::ends;
		filter << L"All Files" << L" (*." << L"*" << L")" << std::ends;
		filter << L"*." << L"*" << std::ends;
		filter << std::ends << std::ends;
		std::wstring filterstr = filter.str();

		// 忘れるとデフォルトファイル名に変な文字列が表示される
		wchar_t filename[256] = L"\0";

		// 構造体を0でクリア
		// ファイルダイアログ設定
		OPENFILENAMEW ofn = { 0 };
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.lpstrFilter = filterstr.c_str();
		ofn.lpstrFile = filename;
		ofn.nMaxFile = sizeof(filename);
		ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		ofn.lpstrDefExt = wextension.c_str();

		// ダイアログ表示
		bool b = (GetSaveFileNameW(&ofn) == TRUE);
		result = string_cast<std::string>(std::wstring(filename));
		return b;
	}

	// 開くダイアログ
	bool OpenDialog(const std::string& extension, const std::string& extensionDesc, std::string& result)
	{
		// フィルター文字列定義
		std::wstringstream filter;
		std::wstring wextension = string_cast<std::wstring>(extension);
		filter << string_cast<std::wstring>(extensionDesc) << L" (*." << wextension << L")" << std::ends;
		filter << L"*." << wextension << std::ends;
		filter << L"All Files" << L" (*." << L"*" << L")" << std::ends;
		filter << L"*." << L"*" << std::ends;
		filter << std::ends << std::ends;
		std::wstring filterstr = filter.str();

		// 忘れるとデフォルトファイル名に変な文字列が表示される
		wchar_t filename[256] = L"\0";

		// 構造体を0でクリア
		// ファイルダイアログ設定
		OPENFILENAMEW ofn = { 0 };
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.lpstrFilter = filterstr.c_str();
		ofn.lpstrFile = filename;
		ofn.nMaxFile = sizeof(filename);
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = wextension.c_str();

		// ダイアログ表示
		bool b = (GetOpenFileNameW(&ofn) == TRUE);
		result = string_cast<std::string>(std::wstring(filename));
		return b;
	}

	// ディレクトリパス取り出し
	std::string GetDirPath(const std::string& name)
	{
		// ディレクトリのパスを除去
		// ピリオドがディレクトリ名に入っていることもあるので先に除去
		const size_t last_slash_idx = name.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			return name.substr(0, last_slash_idx + 1);
		}

		return "";
	}

	// ファイルネーム取り出し
	std::string GetFileName(const std::string& name, const std::string& extension)
	{
		std::string filename = name;

		// ディレクトリのパスを除去
		// ピリオドがディレクトリ名に入っていることもあるので先に除去
		const size_t last_slash_idx = filename.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			filename.erase(0, last_slash_idx + 1);
		}

		// 拡張子があれば取り除く
		{
			const size_t period_idx = filename.rfind("." + extension);
			if (std::string::npos != period_idx)
			{
				filename.erase(period_idx);
			}
		}

		return filename;
	}
}