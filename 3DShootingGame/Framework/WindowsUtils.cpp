#include "pch.h"
#include "WindowsUtils.h"

// 保存ダイアログ
bool WindowsUtils::SaveDialog(const std::string& extension, const std::string& extensionDesc, std::string& result)
{
	// wstring <-> string 変換クラス
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

	// フィルター文字列定義
	std::wstringstream filter;
	std::wstring wextension = cv.from_bytes(extension);
	filter << cv.from_bytes(extensionDesc) << L" (*." << wextension << L")" << std::ends;
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
	result = cv.to_bytes(std::wstring(filename));
	return b;
}

// 開くダイアログ
bool WindowsUtils::OpenDialog(const std::string& extension, const std::string& extensionDesc, std::string& result)
{
	// wstring <-> string 変換クラス
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

	// フィルター文字列定義
	std::wstringstream filter;
	std::wstring wextension = cv.from_bytes(extension);
	filter << cv.from_bytes(extensionDesc) << L" (*." << wextension << L")" << std::ends;
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
	result = cv.to_bytes(std::wstring(filename));
	return b;
}

// ファイルネーム取り出し
std::string WindowsUtils::GetFileName(const std::string& name, const std::string& extension)
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
