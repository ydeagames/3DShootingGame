#include "pch.h"
#include "SceneCommons.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void GeometricObject::Initialize(GameContext& context)
{
	// ジオメトリ作成
	m_pGeometricPrimitive = m_geometricPrimitiveGenerator(context);
}

void GeometricObject::Render(GameContext& context)
{
	// ジオメトリ描画
	m_pGeometricPrimitive->Draw(gameObject->transform->GetMatrix(), context.GetCamera().view, context.GetCamera().projection, m_color);
}

void FontObject::Initialize(GameContext& context)
{
	m_spriteBatch = std::make_unique<SpriteBatch>(context.GetDR().GetD3DDeviceContext());
	m_spriteFont = std::make_unique<SpriteFont>(context.GetDR().GetD3DDevice(), m_fontName.c_str());
}

void FontObject::Render(GameContext& context)
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, context.GetStates().AlphaBlend());
	m_spriteFont->DrawString(m_spriteBatch.get(), m_text.c_str(), *gameObject->transform->position);
	m_spriteBatch->End();
}

void Rigidbody::Update(GameContext& context)
{
	auto elapsed = float(context.GetTimer().GetElapsedSeconds());
	velocity += acceleration * elapsed;
	gameObject->transform->localPosition += velocity * elapsed;
}
