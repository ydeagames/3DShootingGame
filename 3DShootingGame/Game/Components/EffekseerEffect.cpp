#include "pch.h"
#include "EffekseerEffect.h"
#include <Framework/ECS/GameContext.h>
#include <Framework/Context/GameCamera.h>
#include <Framework/Components/Transform.h>
#include "Utilities/StringCast.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// 描画初期化
void EffekseerEffect::RenderStart()
{
	auto str = string_cast<std::u16string>(path);
	
	// エフェクトの読込
	m_effect = effekseer_unique_ptr<Effekseer::Effect>(Effekseer::Effect::Create(GameContext::Get<EffekseerManager>().manager.get(), str.c_str()));

	if (playOnAwake)
		Play(gameObject.GetComponent<Transform>().position);
}

// 描画
Effekseer::Handle EffekseerEffect::Play(const DirectX::SimpleMath::Vector3& pos)
{
	GameContext::Get<EffekseerManager>().manager->Play(m_effect.get(), ToEffekseer(pos));
}

void EffekseerEffect::EditorGui()
{
	{
		std::string tmpname = path;
		tmpname.resize(128);
		if (ImGui::InputText("Path", &tmpname[0], tmpname.size()))
		{
			path = std::string(tmpname.c_str());
			RenderStart();
		}
	}
	if (ImGui::Checkbox("Play On Awake", &playOnAwake))
		RenderStart();
}

