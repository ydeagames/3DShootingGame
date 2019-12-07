#include <Framework/ECS/Component.h>
#include "DebugCamera.h"

class DebugCamera;
class GameCamera;

class DebugCameraWrapper : public Component
{
private:
	// デバッグカメラ
	std::unique_ptr<DebugCamera> m_pDebugCamera;
	// カメラ
	GameCamera* m_camera = nullptr;
public:
	DebugCameraWrapper();
	// Get
	DebugCamera& GetDebugCamera();
	// 生成
	void Initialize();
	// 描画
	void Render(GameCamera& camera);
	// 更新
	void Update();
};