// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-16 22:40:30 +0900
// Modified: 2019-12-16 22:40:30 +0900

#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class FontRenderer : public Component
{
public:
	static constexpr const char* Identifier = "FontRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	// フォントネーム
	std::string fontName = "Resources/Fonts/logofont.spritefont";
	// 表示文字
	std::string text = "Sample Text";
	// 色
	DirectX::SimpleMath::Color color;

private:
	// バッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// フォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

public:
	// 生成
	void RenderStart();
	// 描画
	void RenderGui(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(fontName), CEREAL_OPTIONAL_NVP(text), CEREAL_OPTIONAL_NVP(color));
	}

	void EditorGui();
};


