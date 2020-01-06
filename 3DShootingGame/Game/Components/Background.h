#pragma once
#include <Framework/ECS/Component.h>

class GameCamera;

class Background : public Component
{
public:
	static constexpr const char* Identifier = "Background";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::string texture = "Resources/Textures/Title/Background.png";
	bool fullScreen = true;
	bool uiSpace = true;

private:
	std::unique_ptr<DirectX::GeometricPrimitive> m_plane;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	void RenderStart();
	void Render(GameCamera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_OPTIONAL_NVP(texture), CEREAL_OPTIONAL_NVP(fullScreen), CEREAL_OPTIONAL_NVP(uiSpace));
	}

	void EditorGui();
};

