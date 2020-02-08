#pragma once

class GameCamera;

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

inline Effekseer::Vector3D ToEffekseer(const DirectX::SimpleMath::Vector3& vec)
{
	return Effekseer::Vector3D(vec.x, vec.y, vec.z);
}

inline Effekseer::Matrix44 ToEffekseer(const DirectX::SimpleMath::Matrix& mat)
{
	return reinterpret_cast<const Effekseer::Matrix44&>(mat);
}

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

	void Render(GameCamera& camera);
};
