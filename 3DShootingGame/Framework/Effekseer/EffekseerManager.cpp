#include "pch.h"
#include "EffekseerManager.h"
#include "Framework/ECS/GameContext.h"

EffekseerManager::EffekseerManager()
{
	// 音
	DX::ThrowIfFailed(XAudio2Create(xAudio2.ReleaseAndGetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR));
	// 描画管理インスタンスの生成
	EffekseerRenderer::Renderer* renderer = EffekseerRendererDX11::Renderer::Create(
		GameContext::Get<DX::DeviceResources>().GetD3DDevice(),
		GameContext::Get<DX::DeviceResources>().GetD3DDeviceContext(),
		NumSprites);
	// サウンド管理インスタンスの生成
	EffekseerSound::Sound* sound = ::EffekseerSound::Sound::Create(xAudio2.Get(), 32, 32);
	// エフェクト管理用インスタンスの生成
	Effekseer::Manager* manager = ::Effekseer::Manager::Create(NumSprites);
	
	// 描画方法を指定します。独自に拡張することもできます。
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	// テクスチャ画像の読込方法の指定(パッケージ等から読み込む場合拡張する必要があります。)
	manager->SetTextureLoader(renderer->CreateTextureLoader());

	// サウンド再生用インスタンスの指定
	manager->SetSoundPlayer(sound->CreateSoundPlayer());

	// サウンドデータの読込方法の指定(圧縮フォーマット、パッケージ等から読み込む場合拡張する必要があります。)
	manager->SetSoundLoader(sound->CreateSoundLoader());

	// 座標系の指定(RHで右手系、LHで左手系)
	manager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
}
