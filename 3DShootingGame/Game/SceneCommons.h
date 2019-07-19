#pragma once
#include <Framework/GameObject.h>
#include <Framework/Component.h>
#include <Framework/GameContext.h>
#include <Framework/GameCamera.h>

class GeometricObject : public Component
{
	COMPONENT(Component)

private:
	// ジオメトリプリミティブ
	std::function<std::unique_ptr<DirectX::GeometricPrimitive>(GameContext& context)> m_geometricPrimitiveGenerator;
	// ジオメトリプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_pGeometricPrimitive;
	// 色
	DirectX::SimpleMath::Color m_color;

public:
	// コンストラクタ
	GeometricObject(const std::function<std::unique_ptr<DirectX::GeometricPrimitive>(GameContext& context)>& generator, DirectX::SimpleMath::Color color = DirectX::SimpleMath::Color(DirectX::Colors::Gray))
		: m_geometricPrimitiveGenerator(generator)
		, m_color(color)
	{
	}
	// 生成
	void Initialize(GameContext& context);
	// 描画
	void Render(GameContext& context);
};

class FontObject : public Component
{
	COMPONENT(Component)

private:
	// フォントネーム
	std::wstring m_fontName;
	// フォントネーム
	std::wstring m_text;
	// バッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// フォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	// 色
	DirectX::SimpleMath::Color m_color;

public:
	// コンストラクタ
	FontObject(const std::wstring& fontName, const std::wstring& text, DirectX::SimpleMath::Color color = DirectX::SimpleMath::Color(DirectX::Colors::White))
		: m_fontName(fontName)
		, m_text(text)
		, m_color(color)
	{
	}
	// 生成
	void Initialize(GameContext& context);
	// 描画
	void Render(GameContext& context);

public:
	inline void SetText(const std::wstring& text) { m_text = text; }
	inline const std::wstring GetText() const { return m_text; }
};

class Rigidbody : public Component
{
	COMPONENT(Component)

public:
	// 速度
	DirectX::SimpleMath::Vector3 velocity;
	// 速度
	DirectX::SimpleMath::Vector3 acceleration;

public:
	// コンストラクタ
	Rigidbody()
	{
	}
	// 更新
	void Update(GameContext& context);
};

