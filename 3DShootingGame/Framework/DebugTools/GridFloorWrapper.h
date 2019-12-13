#include <Framework/ECS/Component.h>
#include "GridFloor.h"

class GameCamera;

class GridFloorWrapper : public Component
{
	// グリッド床
	std::unique_ptr<GridFloor> m_pGridFloor;
	// 生成
	void RenderStart();
	// 描画
	void Render(GameCamera& camera);
};
