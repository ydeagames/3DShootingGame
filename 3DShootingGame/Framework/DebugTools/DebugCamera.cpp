//======================================================
// File Name	: DebugCamera.cpp
// Summary		: デバッグカメラ
// Date			: 2019/4/25
// Author		: Takafumi Ban
//======================================================
#include "pch.h"
#include "DebugCamera.h"

// 定数
const float DebugCamera::DEFAULT_CAMERA_DISTANCE = 5.0f;

// コンストラクタ
DebugCamera::DebugCamera()
	: m_yAngle(0.0f), m_xAngle(0.0f), m_prevX(0), m_prevY(0), m_scrollWheelValue(0), m_zoom(-600)
{
}

// デストラクタ
DebugCamera::~DebugCamera()
{
}

// 更新
void DebugCamera::update()
{
	auto state = DirectX::Mouse::Get().GetState();

	// 相対モードなら何もしない
	if (state.positionMode == DirectX::Mouse::MODE_RELATIVE) return;

	m_tracker.Update(state);

	// マウスの左ボタンが押された
	if (m_tracker.leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		// マウスの座標を取得
		m_prevX = state.x;
		m_prevY = state.y;
	}

	// マウスのボタンが押されていたらカメラを移動させる
	if (state.leftButton)
	{
		motion(state.x, state.y);
	}
	// マウスの座標を前回のとして保存
	m_prevX = state.x;
	m_prevY = state.y;

	// マウスのフォイール値を取得
	m_zoom += state.scrollWheelValue - m_scrollWheelValue;
	if (m_zoom >= 500)
	{
		m_zoom = 499;
	}
	m_scrollWheelValue = state.scrollWheelValue;

	// ビュー行列を算出する
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(m_yAngle);
	DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(m_xAngle);

	DirectX::SimpleMath::Matrix rt = rotY * rotX;

	DirectX::SimpleMath::Vector3 eye(0.0f, 0.0f, 1.0f);
	DirectX::SimpleMath::Vector3 target(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);

	eye = DirectX::SimpleMath::Vector3::Transform(eye, rt.Invert());
	eye *= (DEFAULT_CAMERA_DISTANCE - m_zoom / 100);
	up = DirectX::SimpleMath::Vector3::Transform(up, rt.Invert());

	m_eye = eye;
	m_target = target;

	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up);
}

// 行列の生成
void DebugCamera::motion(int x, int y)
{
	// マウスポインタの前回からの変位
	float dx = static_cast<float>(x - m_prevX);
	float dy = static_cast<float>(y - m_prevY);

	if (dx != 0.0f || dy != 0.0f)
	{
		// Ｙ軸の回転
		float yAngle = dx * DirectX::XM_PI / 180.0f;
		// Ｘ軸の回転
		float xAngle = dy * DirectX::XM_PI / 180.0f;

		m_xAngle += xAngle;
		m_yAngle += yAngle;
	}
}

