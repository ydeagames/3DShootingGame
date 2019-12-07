#pragma once

class GameContext;

class PhysXRenderer
{
public:
	static void RenderGeometry(const physx::PxGeometry& geom, const physx::PxMat44& pxworld, const physx::PxVec3& pxcolor, bool wireframe);
	static void RenderGeometryHolder(const physx::PxGeometryHolder& h, const physx::PxMat44& world, const physx::PxVec3& color, bool wireframe);
	static void RenderActors(std::vector<physx::PxRigidActor*>& actors, bool shadows, const physx::PxVec3& color);
};

