#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include <Utilities/Input.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

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

bool SceneManager::IsSceneValid(const std::wstring& name)
{
	return m_sceneBuilders.count(name) > 0;
}

void SceneManager::ProcessScene(GameContext& context)
{
	auto check1 = m_sceneView.scenes.front()->GetName();

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
		if (sceneInfo->destroyed)
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
}

void SceneView::Finalize(GameContext& context)
{
}
