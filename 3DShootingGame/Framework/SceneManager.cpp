#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

SceneManager::SceneManager()
{
	auto sceneInfo = std::make_unique<SceneInfo>();
	sceneInfo->name = "DefaultScene";
	sceneInfo->scene = std::make_unique<Scene>();
	m_sceneInfo = std::move(sceneInfo);
}

void SceneManager::Register(const std::string& name, std::unique_ptr<ISceneBuilder>&& sceneBuilder)
{
	m_sceneBuilders.emplace(name, std::move(sceneBuilder));
}

void SceneManager::LoadScene(const std::string& name)
{
	if (m_sceneBuilders.count(name) < 1)
		throw std::exception("No such scene is registered.");
	m_loadQueue.push(name);
}

void SceneManager::ProcessScene(GameContext& context)
{
	while (!m_loadQueue.empty())
	{
		auto& name = m_loadQueue.front();
		{
			if (m_sceneInfo)
			{
				m_sceneInfo->scene->Finalize(context);
				m_sceneInfo = nullptr;
			}
			{
				auto sceneInfo = std::make_unique<SceneInfo>();
				sceneInfo->name = name;
				sceneInfo->scene = std::make_unique<Scene>();
				m_sceneInfo = std::move(sceneInfo);
			}
			m_sceneBuilders[name]->Build(context);
			m_sceneInfo->scene->Initialize(context);
		}
		m_loadQueue.pop();
	}
}
