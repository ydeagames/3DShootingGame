#pragma once

class GameContext;

class PhysXRenderer
{
public:
	static void RenderGeometry(GameContext& context, const physx::PxGeometry& geom, const physx::PxMat44& pxworld, const physx::PxVec3& pxcolor, bool wireframe);
	static void RenderGeometryHolder(GameContext& context, const physx::PxGeometryHolder& h, const physx::PxMat44& world, const physx::PxVec3& color, bool wireframe);
	static void RenderActors(GameContext& context, std::vector<physx::PxRigidActor*>& actors, bool shadows, const physx::PxVec3& color);
};

