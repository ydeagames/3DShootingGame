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
	// �t�H���g�l�[��
	std::string fontName = "Resources/Fonts/logofont.spritefont";
	// �\������
	std::string text = "Sample Text";
	// �F
	DirectX::SimpleMath::Color color;

private:
	// �o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// �t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

public:
	// ����
	void RenderStart();
	// �`��
	void RenderGui(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(fontName), CEREAL_OPTIONAL_NVP(text), CEREAL_OPTIONAL_NVP(color));
	}

	void EditorGui();
};