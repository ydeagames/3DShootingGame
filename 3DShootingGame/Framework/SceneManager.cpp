#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include <Utilities/Input.h>

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

		void Finalize(GameContext & context)
		{

		}
	};

	std::shared_ptr<SceneTransition> CreateFadeTransition(float duration)
	{
		return std::make_shared<FadeTransition>(duration);
	}
}

SceneManager::SceneManager()
{
	auto scene = std::make_unique<Scene>();
	scene->name = L"DefaultScene";
	m_sceneView.scenes.emplace_back(std::move(scene));
}

void SceneManager::Register(std::unique_ptr<ISceneBuilder>&& sceneBuilder)
{
	m_sceneBuilders.emplace(sceneBuilder->GetName(), std::move(sceneBuilder));
}

void SceneManager::LoadScene(const std::wstring& name, LoadSceneMode mode)
{
	if (m_sceneBuilders.count(name) <= 0)
		throw std::exception("No such scene is registered.");
	m_loadQueue.push(SceneTask{ name, mode });
}

void SceneManager::LoadSceneWithTransition(const std::wstring& name, const std::shared_ptr<SceneTransition>& transition)
{
	m_transitionQueue = transition;
	auto& sceneBefore = GetActiveScene();
	transition->unloadBefore = [&sceneBefore](GameContext & context) { Object::Destroy(sceneBefore); };
	transition->loadAfter = [name](GameContext & context) { context.GetSceneManager().LoadScene(name, LoadSceneMode::Additive); };
}

bool SceneManager::IsSceneValid(const std::wstring& name)
{
	return m_sceneBuilders.count(name) > 0;
}

void SceneManager::ProcessScene(GameContext& context)
{
	auto check1 = m_sceneView.scenes.front()->GetName();

	if (m_transitionQueue)
	{
		m_sceneView.transition = std::move(m_transitionQueue);
		m_transitionQueue = nullptr;
		m_sceneView.transition->Initialize(context);
	}
	if (m_sceneView.transition && m_sceneView.transition->IsDestroyed())
	{
		m_sceneView.transition->Finalize(context);
		m_sceneView.transition = nullptr;
	}

	while (!m_loadQueue.empty())
	{
		auto& task = m_loadQueue.front();
		{
			if (task.mode == LoadSceneMode::Single && !m_sceneView.scenes.empty())
			{
				m_sceneView.scenes.front()->Finalize(context);
				m_sceneView.scenes.pop_front();
			}
			{
				auto scene = std::make_unique<Scene>();
				scene->name = task.name;
				m_sceneView.scenes.push_front(std::move(scene));
			}
			m_sceneBuilders[task.name]->Build(context);
			m_sceneView.scenes.front()->Initialize(context);
		}
		m_loadQueue.pop();
	}

	for (auto itr = m_sceneView.scenes.begin(); itr != m_sceneView.scenes.end();)
	{
		auto& sceneInfo = *itr;
		if (sceneInfo->IsDestroyed())
		{
			sceneInfo->Finalize(context);
			itr = m_sceneView.scenes.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	auto check2 = m_sceneView.scenes.front()->GetName();
	if (check1 != check2)
	{
		Input::SetMouseMode(m_sceneView.scenes.front()->mouseMode);
	}
}

void SceneView::Initialize(GameContext& context)
{
}

void SceneView::Update(GameContext& context)
{
	auto itr = scenes.begin();
	for (;itr != scenes.end(); ++itr)
	{
		if (!(*itr)->updateBehind)
			break;
	}
	for (;;itr--)
	{
		(*itr)->Update(context);
		if (itr == scenes.begin())
			break;
	}

	if (transition)
		transition->Update(context);
}

void SceneView::Render(GameContext& context)
{
	auto itr = scenes.begin();
	for (; itr != scenes.end(); ++itr)
	{
		if (!(*itr)->renderBehind)
			break;
	}
	for (;; itr--)
	{
		(*itr)->Render(context);
		if (itr == scenes.begin())
			break;
	}

	if (transition)
		transition->Render(context);
}

void SceneView::Finalize(GameContext& context)
{
}
