// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-08-05 07:35:38 +0900
// Modified: 2019-08-05 07:35:38 +0900

/// <summary>
/// バイナリファイルを扱うクラス
/// </summary>
#pragma once

#include <memory>

class BinaryFile
{
public:
	// ファイル名を指定してロード
	static BinaryFile LoadFile(const wchar_t* fileName);

	BinaryFile();
	// ムーブコンストラクタ
	BinaryFile(BinaryFile&& in);

	// アクセッサ
	char* GetData() { return m_Data.get(); }
	unsigned int GetSize() { return m_Size; }

protected:
	// データ
	std::unique_ptr<char[]> m_Data;
	// サイズ
	unsigned int m_Size;
};


