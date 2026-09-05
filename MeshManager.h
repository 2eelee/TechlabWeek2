#pragma once

#include "URenderer.h"
#include "Vertices.h"
#include <d3d11.h>

class MeshManager
{
public:
	static MeshManager& Get()
	{
		static MeshManager Instance;
		return Instance;
	}

	void Initialize(URenderer& renderer)
	{
		PlaneVertexBuffer = renderer.CreateVertexBuffer(plane_vertices, sizeof(plane_vertices));
		SphereVertexBuffer = renderer.CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
		CubeVertexBuffer = renderer.CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));
	}

	void Release(URenderer& renderer)
	{
		renderer.ReleaseVertexBuffer(PlaneVertexBuffer);
		renderer.ReleaseVertexBuffer(CubeVertexBuffer);
		renderer.ReleaseVertexBuffer(SphereVertexBuffer);
	}

	ID3D11Buffer* GetPlaneVertexBuffer() const { return PlaneVertexBuffer; }
	ID3D11Buffer* GetCubeVertexBuffer() const { return CubeVertexBuffer; }
	ID3D11Buffer* GetSphereVertexBuffer() const { return SphereVertexBuffer; }

	const UINT PlaneVertexCount = sizeof(plane_vertices) / sizeof(FVertexSimple);
	const UINT CubeVertexCount = sizeof(cube_vertices) / sizeof(FVertexSimple);
	const UINT SphereVertexCount = sizeof(sphere_vertices) / sizeof(FVertexSimple);
	
private:
	MeshManager() = default;

	ID3D11Buffer* PlaneVertexBuffer = nullptr;
	ID3D11Buffer* CubeVertexBuffer = nullptr;
	ID3D11Buffer* SphereVertexBuffer = nullptr;
};