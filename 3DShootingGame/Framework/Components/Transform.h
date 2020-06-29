// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2020-01-05 22:47:30 +0900

#pragma once
#include <Framework/ECS/Component.h>

class TransformResolver;

// トランスフォーム
class Transform : public Component
{
public:
	static constexpr const char* Identifier = "Transform";

public:
	// オブジェクトの名前
	std::string name;
	// 親オブジェクトの参照
	entt::entity parent = entt::null;
	// 静的か
	bool isStatic = false;

public:
	// ローカル位置
	DirectX::SimpleMath::Vector3 localPosition;
	// ローカル回転
	DirectX::SimpleMath::Quaternion localRotation;
	// ローカルスケール
	DirectX::SimpleMath::Vector3 localScale;

private:
	// 親子関係解決
	TransformResolver* transformResolver = nullptr;

public:
	// コンストラクタ
	Transform::Transform();

public:
	// 初期化
	void Awake();

	// グローバル位置
	DirectX::SimpleMath::Vector3 GetPosition() const;
	void SetPosition(const DirectX::SimpleMath::Vector3& value);
	__declspec(property(get = GetPosition, put = SetPosition)) DirectX::SimpleMath::Vector3 position;

	// グローバル回転
	DirectX::SimpleMath::Quaternion GetRotation() const;
	void SetRotation(const DirectX::SimpleMath::Quaternion& value);
	__declspec(property(get = GetRotation, put = SetRotation)) DirectX::SimpleMath::Quaternion rotation;

	// グローバルスケール
	DirectX::SimpleMath::Vector3 GetLossyScale() const;
	void SetLossyScale(const DirectX::SimpleMath::Vector3& value);
	__declspec(property(get = GetLossyScale, put = SetLossyScale)) DirectX::SimpleMath::Vector3 lossyScale;

	// ローカル行列
	DirectX::SimpleMath::Matrix GetLocalMatrix() const;
	// グローバル行列
	DirectX::SimpleMath::Matrix GetMatrix() const;

public:
	// シリアライズ
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(name), CEREAL_OPTIONAL_NVP(parent), CEREAL_OPTIONAL_NVP(isStatic));
		archive(CEREAL_OPTIONAL_NVP(localPosition), CEREAL_OPTIONAL_NVP(localRotation), CEREAL_OPTIONAL_NVP(localScale));
	}

	// コンポーネント参照
	template<class Reference>
	void Reference(Reference& reference)
	{
		reference(parent);
	}

	// エディター
	void EditorGui();
};

// 親子関係解決クラス - 親子関係の行列を求めるクラス
class TransformResolver
{
private:
	// 計算結果はキャッシュし、フレームの最初にクリアする
	entt::SparseSet<entt::entity, DirectX::SimpleMath::Matrix> matrixMap;

public:
	// キャッシュをクリア
	void ClearCache();
	// グローバル行列を計算
	DirectX::SimpleMath::Matrix Resolve(const GameObject& transform);
};

// 親子関係解決クラスの管理
class TransformResolverContext
{
private:
	// レジストリ(シーン)ごとに管理
	std::unordered_map<entt::registry*, TransformResolver> resolvers;

public:
	// すべてのシーン用のキャッシュをクリア
	void ClearCache();
	// 親子関係解決クラスを取得
	TransformResolver& GetResolver(entt::registry& registry);
};

