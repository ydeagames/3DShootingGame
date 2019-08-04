#include "pch.h"
#include "SceneTransitions.h"
#include <Framework/GameContext.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace SceneTransitions
{
	// フェード実装
	struct FadeTransition : public SceneTransition
	{
		float m_time;
		float m_totalTime;

		FadeTransition(float totalTime)
			: m_time(0)
			, m_totalTime(totalTime) {}

		std::unique_ptr<GeometricPrimitive> m_plane;
		bool changed = false;

		void Initialize(GameContext& context)
		{
			std::vector<GeometricPrimitive::VertexType> vertices = {
				{ Vector3(-0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 0.0f) },
				{ Vector3(+0.5f, +0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 0.0f) },
				{ Vector3(+0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(1.0f, 1.0f) },
				{ Vector3(-0.5f, -0.5f, 0.0f), Vector3::Forward, Vector2(0.0f, 1.0f) },
			};
			std::vector<uint16_t> indices = {
				0, 1, 2, 0, 2, 3,
			};
			m_plane = GeometricPrimitive::CreateCustom(context.GetDR().GetD3DDeviceContext(), vertices, indices);
		}

		void Update(GameContext& context)
		{
			m_time += float(context.GetTimer().GetElapsedSeconds());
			if (!changed && m_time >= m_totalTime / 2)
			{
				unloadBefore(context);
				loadAfter(context);
				changed = true;
			}
			if (m_time > m_totalTime)
				Destroy(*this);
		}

		void Render(GameContext& context)
		{
			float alpha = std::abs((m_time / m_totalTime - .5f) * 2);
			alpha = 1 - alpha * alpha;
			m_plane->Draw(Matrix::CreateScale(2), Matrix::Identity, Matrix::Identity, Colors::Black * alpha);
		}

		void Finalize(GameContext& context)
		{

		}
	};

	std::shared_ptr<SceneTransition> CreateFadeTransition(float duration)
	{
		return std::make_shared<FadeTransition>(duration);
	}
}
