//======================================================
// File Name	: DebugCamera.h
// Summary		: デバッグカメラ
// Date			: 2019/4/25
// Author		: Takafumi Ban
//======================================================
#pragma once

#include "SimpleMath.h"
#include "Mouse.h"

// デバッグ用カメラクラス
class DebugCamera
{
public: // 定数

	// カメラの距離
	static const float DEFAULT_CAMERA_DISTANCE;

public: // 基本

	// コンストラクタ
	DebugCamera();
	// デストラクタ
	~DebugCamera();

public: // 基本処理

	// 更新
	void update();

public: // アクセッサ

	// ビュー行列取得
	DirectX::SimpleMath::Matrix getViewMatrix()
	{
		return m_view;
	}

	// デバッグカメラの位置取得
	DirectX::SimpleMath::Vector3 getEyePosition()
	{
		return m_eye;
	}
	// デバッグカメラの注視点取得
	DirectX::SimpleMath::Vector3 getTargetPosition()
	{
		return m_target;
	}

private: // サブ処理
	void motion(int x, int y);

public:
	// 横回転
	float m_yAngle;

	// 縦回転
	float m_xAngle;

private: // 変数
	// 前回のマウス座標
	int m_prevX, m_prevY;

	// 生成されたビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// スクロールフォイール値
	int m_scrollWheelValue;
	// スクロールフォイール値
	int m_zoom;

	// 視点
	DirectX::SimpleMath::Vector3 m_eye;

	// 注視点
	DirectX::SimpleMath::Vector3 m_target;
	
	// マウストラッカー
	DirectX::Mouse::ButtonStateTracker m_tracker;
};
