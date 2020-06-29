// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 19:20:01 +0900
// Modified: 2019-12-16 05:32:44 +0900

#pragma once

class GameCamera;

class PhysXRenderer
{
public:
	static void RenderGeometry(GameCamera& camera, const physx::PxGeometry& geom, const physx::PxMat44& pxworld, const physx::PxVec3& pxcolor, bool wireframe);
	static void RenderGeometryHolder(GameCamera& camera, const physx::PxGeometryHolder& h, const physx::PxMat44& world, const physx::PxVec3& color, bool wireframe);
	static void RenderActors(GameCamera& camera, std::vector<physx::PxRigidActor*>& actors, bool shadows, const physx::PxVec3& color);
};



