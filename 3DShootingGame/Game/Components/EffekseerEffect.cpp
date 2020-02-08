#include "pch.h"
#include "EffekseerEffect.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>
#include <Utilities/MathUtils.h>
#include "Utilities/StringCast.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// 描画初期化
void EffekseerEffect::RenderStart()
{
	auto str = string_cast<std::wstring>(path);
	
	// エフェクトの読込
	Effekseer::Effect* effect = Effekseer::Effect::Create(GameContext::Get<EffekseerManager>().manager.get(), L"");
}

// 描画
void EffekseerEffect::Render(GameCamera& camera)
{
}

void EffekseerEffect::EditorGui()
{
	ImGui::DragInt("Path", &path);
	ImGui::Checkbox("Play On Awake", &playOnAwake);
}

