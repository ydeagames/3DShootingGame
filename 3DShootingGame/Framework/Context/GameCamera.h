// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2020-01-15 16:08:06 +0900

#pragma once

// カメラ
class GameCamera
{
public:
	// ビュー行列
	DirectX::SimpleMath::Matrix view;
	// プロジェクション行列
	DirectX::SimpleMath::Matrix projection;
	// ビューポート座標
	DirectX::SimpleMath::Matrix viewport;

public:
	// 位置取得
	DirectX::SimpleMath::Vector3 GetPosition() const;
	// 回転取得
	DirectX::SimpleMath::Quaternion GetRotation() const;
	// 画面のサイズ
	DirectX::SimpleMath::Vector3 GetWindowSize() const;

	// スクリーン座標からRayに変換
	DirectX::SimpleMath::Ray ScreenPointToRay(const DirectX::SimpleMath::Vector3& point) const;
	// スクリーン座標からRayに変換
	DirectX::SimpleMath::Ray ViewportPointToRay(const DirectX::SimpleMath::Vector3& point) const;

	// ワールド座標からスクリーン座標に変換
	DirectX::SimpleMath::Vector3 WorldToScreenPoint(const DirectX::SimpleMath::Vector3& point) const;
	// ワールド座標からビューポート座標に変換
	DirectX::SimpleMath::Vector3 WorldToViewportPoint(const DirectX::SimpleMath::Vector3& point) const;
	// ビューポート座標からワールド座標に変換
	DirectX::SimpleMath::Vector3 ViewportToWorldPoint(const DirectX::SimpleMath::Vector3& point) const;
	// スクリーン座標からワールド座標に変換
	DirectX::SimpleMath::Vector3 ScreenToWorldPoint(const DirectX::SimpleMath::Vector3& point) const;
	// スクリーン座標からビューポート座標に変換
	DirectX::SimpleMath::Vector3 ScreenToViewportPoint(const DirectX::SimpleMath::Vector3& point) const;
	// ビューポート座標からスクリーン座標に変換
	DirectX::SimpleMath::Vector3 ViewportToScreenPoint(const DirectX::SimpleMath::Vector3& point) const;
};

