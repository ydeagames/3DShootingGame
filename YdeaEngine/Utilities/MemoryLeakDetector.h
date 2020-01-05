#pragma once

// ヘッダファイルの読み込み
#if defined(_DEBUG)
	#include <crtdbg.h>
#endif

// マクロ定義
#if defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// インライン関数定義
#if defined(_DEBUG)
	// メモリーリーク検出機構のセットアップ
	inline void SetUpMemoryLeakDetector()
	{
		// メモリーリーク検出のための設定
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	}
#endif
