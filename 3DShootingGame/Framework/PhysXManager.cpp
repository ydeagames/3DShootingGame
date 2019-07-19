#include "pch.h"
#include "PhysXManager.h"
#include "PhysXScene.h"
#include "GameContext.h"
#include "GameCamera.h"
#include <Common/StepTimer.h>

using namespace physx;
using namespace DirectX;
using namespace DirectX::SimpleMath;

PhysXManager::PhysXManager()
{
}

PhysXManager::~PhysXManager()
{
}

void PhysXManager::Initialize(GameContext& context)
{
	m_foundation = px_unique_ptr<PxFoundation>(PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback));

	m_pvd = px_unique_ptr<PxPvd>(PxCreatePvd(*m_foundation));
	m_pvdTransport = px_unique_ptr<PxPvdTransport>(PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10));
	m_pvd->connect(*m_pvdTransport, PxPvdInstrumentationFlag::eALL);

	m_physics = px_unique_ptr<PxPhysics>(PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, m_pvd.get()));

	m_dispatcher = px_unique_ptr<PxDefaultCpuDispatcher>(PxDefaultCpuDispatcherCreate(2));
}

void PhysXManager::Update(GameContext& context)
{
	auto nbScenes = m_physics->getNbScenes();
	if (nbScenes)
	{
		std::vector<physx::PxScene*> scenes(nbScenes);
		m_physics->getScenes(&scenes[0], nbScenes);

		for (auto& scene : scenes)
		{
			//scene->simulate(1.0f / 60.0f);
			scene->simulate(float(context.GetTimer().GetElapsedSeconds()));
			scene->fetchResults(true);
		}
	}
}

namespace
{
	using namespace physx;

	using VertexCollection = std::vector<DirectX::VertexPositionNormalTexture>;
	using IndexCollection = std::vector<uint16_t>;

	inline void CheckIndexOverflow(size_t value)
	{
		// Use >=, not > comparison, because some D3D level 9_x hardware does not support 0xFFFF index values.
		if (value >= USHRT_MAX)
			throw std::exception("Index value out of range: cannot tesselate primitive so finely");
	}

	// Collection types used when generating the geometry.
	inline void index_push_back(IndexCollection& indices, size_t value)
	{
		CheckIndexOverflow(value);
		indices.push_back(static_cast<uint16_t>(value));
	}

	// Helper computes a point on a unit circle, aligned to the x/z plane and centered on the origin.
	inline Vector3 GetCircleVector(size_t i, size_t tessellation)
	{
		float angle = i * XM_2PI / tessellation;
		float dx, dz;

		XMScalarSinCos(&dx, &dz, angle);

		return { dx, 0, dz };
	}

	// Helper for flipping winding of geometric primitives for LH vs. RH coords
	inline void ReverseWinding(IndexCollection& indices, VertexCollection& vertices)
	{
		assert((indices.size() % 3) == 0);
		for (auto it = indices.begin(); it != indices.end(); it += 3)
		{
			std::swap(*it, *(it + 2));
		}

		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			it->textureCoordinate.x = (1.f - it->textureCoordinate.x);
		}
	}

	// Helper creates a triangle fan to close the end of a cylinder / cone
	void CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height, float radius, const Quaternion& rotation)
	{
		// Create cap indices.
		for (size_t i = 0; i < tessellation; i++)
		{
			size_t i1 = (i + 0) % tessellation;
			size_t i2 = (i + 1) % tessellation;

			size_t vbase = vertices.size();
			index_push_back(indices, vbase + i2);
			index_push_back(indices, vbase + i1);
			index_push_back(indices, vbase + i2);
		}

		// Which end of the cylinder is this?
		auto normal = Vector3::Up;
		auto textureScale = Vector3::One * -.5f;

		// Create cap vertices.
		for (size_t i = 0; i < tessellation; i++)
		{
			auto circleVector = GetCircleVector(i, tessellation);

			auto position = circleVector * radius;

			auto textureCoordinate = Vector3(XMVectorSwizzle<0, 2, 3, 3>(circleVector)) * textureScale + Vector3::One * .5f;

			vertices.push_back(VertexPositionNormalTexture(Vector3::Transform(position, rotation), Vector3::Transform(normal, rotation), textureCoordinate));
		}
	}

	void ComputeCircle(VertexCollection& vertices, IndexCollection& indices, float radius, size_t tessellation, bool rhcoords)
	{
		vertices.clear();
		indices.clear();

		if (tessellation < 3)
			throw std::out_of_range("tesselation parameter out of range");

		float height = 5;
		height /= 2;

		// Create flat triangle fan caps to seal the top and bottom.
		CreateCylinderCap(vertices, indices, tessellation, height, radius, Quaternion::Identity);
		CreateCylinderCap(vertices, indices, tessellation, height, radius, Quaternion::CreateFromAxisAngle(Vector3::UnitX, XM_PIDIV2));
		CreateCylinderCap(vertices, indices, tessellation, height, radius, Quaternion::CreateFromAxisAngle(Vector3::UnitZ, XM_PIDIV2));

		// Build RH above
		if (!rhcoords)
			ReverseWinding(indices, vertices);
	}

	std::unique_ptr<GeometricPrimitive> CreateSphereDisplay(ID3D11DeviceContext* deviceContext, float radius = .5f, size_t tessellation = 32, bool rhcoords = true)
	{
		VertexCollection vertices;
		IndexCollection indices;

		ComputeCircle(vertices, indices, radius, tessellation, rhcoords);

		return GeometricPrimitive::CreateCustom(deviceContext, vertices, indices);
	}

	void renderGeometry(GameContext& context, const PxGeometry& geom, const PxMat44& pxworld, const PxVec3& pxcolor, bool wireframe)
	{
		static auto cube = GeometricPrimitive::CreateCube(context.GetDR().GetD3DDeviceContext());
		static auto sphere = CreateSphereDisplay(context.GetDR().GetD3DDeviceContext());

		Matrix world = fromPhysX(pxworld);
		Vector4 color = Vector4(pxcolor.x, pxcolor.y, pxcolor.z, .5f);
		switch (geom.getType())
		{
		case PxGeometryType::eBOX:
		{
			const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
			cube->Draw(Matrix::CreateScale(fromPhysX(boxGeom.halfExtents) * 2) * world, context.GetCamera().view, context.GetCamera().projection, color, nullptr, wireframe);
		}
		break;

		case PxGeometryType::eSPHERE:
		{
			const PxSphereGeometry& sphereGeom = static_cast<const PxSphereGeometry&>(geom);
			sphere->Draw(Matrix::CreateScale(sphereGeom.radius * 2) * world, context.GetCamera().view, context.GetCamera().projection, color, nullptr, wireframe);
		}
		break;

		/*
		case PxGeometryType::eCAPSULE:
		{
			const PxCapsuleGeometry& capsuleGeom = static_cast<const PxCapsuleGeometry&>(geom);
			const PxF32 radius = capsuleGeom.radius;
			const PxF32 halfHeight = capsuleGeom.halfHeight;

			//Sphere
			glPushMatrix();
			glTranslatef(halfHeight, 0.0f, 0.0f);
			glScalef(radius, radius, radius);
			glutSolidSphere(1, 10, 10);
			glPopMatrix();

			//Sphere
			glPushMatrix();
			glTranslatef(-halfHeight, 0.0f, 0.0f);
			glScalef(radius, radius, radius);
			glutSolidSphere(1, 10, 10);
			glPopMatrix();

			//Cylinder
			glPushMatrix();
			glTranslatef(-halfHeight, 0.0f, 0.0f);
			glScalef(2.0f*halfHeight, radius, radius);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gCylinderData);
			glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gCylinderData + 3);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 13 * 2);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glPopMatrix();
		}
		break;

		case PxGeometryType::eCONVEXMESH:
		{
			const PxConvexMeshGeometry& convexGeom = static_cast<const PxConvexMeshGeometry&>(geom);

			//Compute triangles for each polygon.
			const PxVec3& scale = convexGeom.scale.scale;
			PxConvexMesh* mesh = convexGeom.convexMesh;
			const PxU32 nbPolys = mesh->getNbPolygons();
			const PxU8* polygons = mesh->getIndexBuffer();
			const PxVec3* verts = mesh->getVertices();
			PxU32 nbVerts = mesh->getNbVertices();
			PX_UNUSED(nbVerts);

			PxU32 numTotalTriangles = 0;
			for (PxU32 i = 0; i < nbPolys; i++)
			{
				PxHullPolygon data;
				mesh->getPolygonData(i, data);

				const PxU32 nbTris = PxU32(data.mNbVerts - 2);
				const PxU8 vref0 = polygons[data.mIndexBase + 0];
				PX_ASSERT(vref0 < nbVerts);
				for (PxU32 j = 0; j<nbTris; j++)
				{
					const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
					const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

					//generate face normal:
					PxVec3 e0 = verts[vref1] - verts[vref0];
					PxVec3 e1 = verts[vref2] - verts[vref0];

					PX_ASSERT(vref1 < nbVerts);
					PX_ASSERT(vref2 < nbVerts);

					PxVec3 fnormal = e0.cross(e1);
					fnormal.normalize();

					if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
					{
						gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
						gVertexBuffer[numTotalTriangles * 6 + 1] = verts[vref0];
						gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
						gVertexBuffer[numTotalTriangles * 6 + 3] = verts[vref1];
						gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
						gVertexBuffer[numTotalTriangles * 6 + 5] = verts[vref2];
						numTotalTriangles++;
					}
				}
			}
			glPushMatrix();
			glScalef(scale.x, scale.y, scale.z);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
			glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
			glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
			glPopMatrix();
		}
		break;

		case PxGeometryType::eTRIANGLEMESH:
		{
			const PxTriangleMeshGeometry& triGeom = static_cast<const PxTriangleMeshGeometry&>(geom);

			const PxTriangleMesh& mesh = *triGeom.triangleMesh;
			const PxVec3 scale = triGeom.scale.scale;

			const PxU32 triangleCount = mesh.getNbTriangles();
			const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
			const void* indexBuffer = mesh.getTriangles();

			const PxVec3* vertexBuffer = mesh.getVertices();

			const PxU32* intIndices = reinterpret_cast<const PxU32*>(indexBuffer);
			const PxU16* shortIndices = reinterpret_cast<const PxU16*>(indexBuffer);
			PxU32 numTotalTriangles = 0;
			for (PxU32 i = 0; i < triangleCount; ++i)
			{
				PxVec3 triVert[3];

				if (has16BitIndices)
				{
					triVert[0] = vertexBuffer[*shortIndices++];
					triVert[1] = vertexBuffer[*shortIndices++];
					triVert[2] = vertexBuffer[*shortIndices++];
				}
				else
				{
					triVert[0] = vertexBuffer[*intIndices++];
					triVert[1] = vertexBuffer[*intIndices++];
					triVert[2] = vertexBuffer[*intIndices++];
				}

				PxVec3 fnormal = (triVert[1] - triVert[0]).cross(triVert[2] - triVert[0]);
				fnormal.normalize();

				if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
				{
					gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
					gVertexBuffer[numTotalTriangles * 6 + 1] = triVert[0];
					gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
					gVertexBuffer[numTotalTriangles * 6 + 3] = triVert[1];
					gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
					gVertexBuffer[numTotalTriangles * 6 + 5] = triVert[2];
					numTotalTriangles++;
				}
			}
			glPushMatrix();
			glScalef(scale.x, scale.y, scale.z);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
			glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
			glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glPopMatrix();
		}
		break;
		*/

		case PxGeometryType::eINVALID:
		case PxGeometryType::eHEIGHTFIELD:
		case PxGeometryType::eGEOMETRY_COUNT:
		case PxGeometryType::ePLANE:
			break;
		}
	}

	void renderGeometryHolder(GameContext& context, const PxGeometryHolder& h, const PxMat44& world, const PxVec3& color, bool wireframe)
	{
		renderGeometry(context, h.any(), world, color, wireframe);
	}

	void renderActors(GameContext& context, std::vector<PxRigidActor*>& actors, bool shadows, const PxVec3& color)
	{
		const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
		//const PxMat44 shadowMat = PxMat44
		//{
		//	{ 1, 0, 0, 0 },
		//	{ -shadowDir.x / shadowDir.y, 0, -shadowDir.z / shadowDir.y, 0 },
		//	{ 0, 0, 1, 0 },
		//	{ 0, 0, 0, 1 },
		//};

		std::vector<PxShape*> shapes;
		for (auto& actor : actors)
		{
			const PxU32 nbShapes = actor->getNbShapes();
			shapes.resize(nbShapes);
			actor->getShapes(shapes.data(), nbShapes);
			const bool sleeping = actor->is<PxRigidDynamic>() ? actor->is<PxRigidDynamic>()->isSleeping() : false;

			for (auto& shape : shapes)
			{
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shape, *actor));
				const PxGeometryHolder h = shape->getGeometry();

				bool wireframe = true; // shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE;

				// render object
				PxVec3 col = color;
				if (sleeping)
					col = color * 0.25f;

				renderGeometryHolder(context, h, shapePose, col, wireframe);

				//if (shadows)
				//{
				//	//glDisable(GL_LIGHTING);
				//	renderGeometryHolder(context, h, shadowMat * shapePose, PxVec3(0.1f, 0.2f, 0.3f), false);
				//	//glEnable(GL_LIGHTING);
				//}
			}
		}
	}
}

void PhysXManager::Render(GameContext& context)
{
	auto nbScenes = m_physics->getNbScenes();
	if (nbScenes)
	{
		std::vector<physx::PxScene*> scenes(nbScenes);
		m_physics->getScenes(&scenes[0], nbScenes);

		for (auto& scene : scenes)
		{
			PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
			if (nbActors)
			{
				std::vector<PxRigidActor*> actors(nbActors);
				scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
				renderActors(context, actors, true, physx::PxVec3(0, 0, 1));
			}
		}
	}
}

void PhysXManager::Finalize(GameContext& context)
{
}

std::unique_ptr<PhysXScene> PhysXManager::CreateScene()
{
	PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_dispatcher.get();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	return std::make_unique<PhysXScene>(m_physics->createScene(sceneDesc));
}
