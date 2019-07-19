#include <Framework/GameObject.h>
#include "DebugCamera.h"

class DebugCamera;

class DebugCameraWrapper : public Component
{
private:
	// デバッグカメラ
	std::unique_ptr<DebugCamera> m_pDebugCamera;
public:
	DebugCameraWrapper();
	// Get
	DebugCamera& GetDebugCamera();
	// 生成
	void Initialize(GameContext& context);
	// 更新
	void Update(GameContext& context);
};