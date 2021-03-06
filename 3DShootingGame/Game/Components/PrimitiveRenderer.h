// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-09 14:22:32 +0900
// Modified: 2020-02-05 23:03:19 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class PrimitiveRenderer : public Component
{
public:
	static constexpr const char* Identifier = "PrimitiveRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	class PrimitiveModel
	{
	public:
		using generator_type = std::function<std::unique_ptr<DirectX::GeometricPrimitive>(ID3D11DeviceContext* device)>;

	private:
		generator_type gen;
		generator_type invgen;
		std::unique_ptr<DirectX::GeometricPrimitive> model;
		std::unique_ptr<DirectX::GeometricPrimitive> invmodel;

	public:
		PrimitiveModel(const generator_type& gen, const generator_type& invgen)
			: gen(gen)
			, invgen(invgen)
		{
		}

	public:
		DirectX::GeometricPrimitive* GetOrCreate(ID3D11DeviceContext* device)
		{
			if (!model)
				model = gen(device);
			return model.get();
		}

		DirectX::GeometricPrimitive* GetOrCreateInverted(ID3D11DeviceContext* device)
		{
			if (!invmodel)
				invmodel = invgen(device);
			return invmodel.get();
		}
	};

	std::unordered_map<std::string, PrimitiveModel>& primitiveModels();

public:
	std::string model = "Teapot";
	std::string texture = "Resources/Textures/Title/Background.png";
	bool lighting = true;
	bool cullfront = false;
	bool textureEnabled = true;

private:
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// モデル
	DirectX::GeometricPrimitive* m_model = nullptr;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void RenderStart();
	void RenderShadow(GameCamera& camera, bool shadowMode);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(model), CEREAL_OPTIONAL_NVP(texture),
			CEREAL_OPTIONAL_NVP(lighting), CEREAL_OPTIONAL_NVP(cullfront), CEREAL_OPTIONAL_NVP(textureEnabled));
	}

	void EditorGui();
};


