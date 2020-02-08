#pragma once

template<typename T>
inline void effekseer_release(T* t)
{
	if (t)
		t->release();
}

template<typename T>
struct effekseer_deleter
{
	void operator()(T* t)
	{
		px_release(t);
	}
};

template<typename T>
using effekseer_unique_ptr = std::unique_ptr<T, effekseer_deleter<T>>;

template<typename T>
struct effekseer_shared_ptr : public std::shared_ptr<T>
{
	effekseer_shared_ptr(T* _Px)
		: std::shared_ptr(_Px, effekseer_deleter<T>)
	{
	}
};

class EffekseerManager
{
public:
	static constexpr int NumSprites = 128;
	
public:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	effekseer_unique_ptr<EffekseerRenderer::Renderer> renderer;
	effekseer_unique_ptr<EffekseerSound::Sound> sound;
	effekseer_unique_ptr<Effekseer::Manager> manager;

public:
	EffekseerManager();
};
