#pragma once

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