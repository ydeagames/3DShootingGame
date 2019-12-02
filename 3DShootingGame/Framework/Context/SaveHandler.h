#pragma once

class SaveHandler
{
private:
	std::wstring m_saveDir;

public:
	SaveHandler(const std::wstring& saveDir)
		: m_saveDir(saveDir)
	{
		CreateDirectoryW(saveDir.c_str(), nullptr);
	}

	std::wstring GetDir()
	{
		return m_saveDir;
	}
};

