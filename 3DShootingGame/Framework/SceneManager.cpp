#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

SceneManager::SceneManager()
{
	auto sceneInfo = std::make_unique<SceneInfo>();
	sceneInfo->name = L"DefaultScene";
	sceneInfo->scene = std::make_unique<Scene>();
	m_sceneView.scenes.emplace_back(std::move(sceneInfo));
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

bool SceneManager::IsSceneValid(const std::wstring& name)
{
	return m_sceneBuilders.count(name) > 0;
}

void SceneManager::ProcessScene(GameContext& context)
{
	while (!m_loadQueue.empty())
	{
		auto& task = m_loadQueue.front();
		{
			if (task.mode == LoadSceneMode::Single && !m_sceneView.scenes.empty())
			{
				m_sceneView.scenes.front()->scene->Finalize(context);
				m_sceneView.scenes.pop_front();
			}
			{
				auto sceneInfo = std::make_unique<SceneInfo>();
				sceneInfo->name = task.name;
				sceneInfo->scene = std::make_unique<Scene>();
				m_sceneView.scenes.push_front(std::move(sceneInfo));
			}
			m_sceneBuilders[task.name]->Build(context);
			m_sceneView.scenes.front()->scene->Initialize(context);
		}
		m_loadQueue.pop();
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
		if (!(*itr)->scene->updateBehind)
			break;
	}
	for (;;itr--)
	{
		(*itr)->scene->Update(context);
		if (itr == scenes.begin())
			break;
	}
}

void SceneView::Render(GameContext& context)
{
	auto itr = scenes.begin();
	for (; itr != scenes.end(); ++itr)
	{
		if (!(*itr)->scene->renderBehind)
			break;
	}
	for (;; itr--)
	{
		(*itr)->scene->Render(context);
		if (itr == scenes.begin())
			break;
	}
}

void SceneView::Finalize(GameContext& context)
{
}
