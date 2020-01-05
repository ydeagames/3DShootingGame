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

