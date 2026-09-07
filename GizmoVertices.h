#pragma once

#include <vector>
#include "FVertexSimple.h"

namespace GizmoGeometry
{
	inline constexpr float ShaftLength = 0.95f;
	inline constexpr float ShaftHalfWidth = 0.03f;

	// Translate
	inline constexpr float TranslateHeadLength = 1.15f;
	inline constexpr float TranslateHeadHalfWidth = 0.10f;

	// Scale
	inline constexpr float ScaleHeadMin = 0.90f;
	inline constexpr float ScaleHeadMax = 1.12f;
	inline constexpr float ScaleHeadHalfWidth = 0.11f;

	// Rotate
	inline constexpr int RotateSegments = 64;
	inline constexpr float RotateOuterRadius = 1.35f;
	inline constexpr float RotateInnerRadius = 1.31f;
}

inline std::vector<FVertexSimple> CreateCheckerGridVertices()
{
	std::vector<FVertexSimple> vertices;

	constexpr int HalfSize = 10;

	for (int x = -HalfSize; x < HalfSize; ++x)
	{
		for (int z = -HalfSize; z < HalfSize; ++z)
		{
			float x0 = static_cast<float>(x);
			float x1 = static_cast<float>(x + 1);
			float z0 = static_cast<float>(z);
			float z1 = static_cast<float>(z + 1);

			float c = ((x + z) % 2 == 0) ? 0.16f : 0.20f;
			float a = 0.35f; // 바둑판 투명도 후보

			// 삼각형 1
			vertices.push_back({ x0, 0.0f, z0, c, c, c, a });
			vertices.push_back({ x1, 0.0f, z0, c, c, c, a });
			vertices.push_back({ x1, 0.0f, z1, c, c, c, a });

			// 삼각형 2
			vertices.push_back({ x0, 0.0f, z0, c, c, c, a });
			vertices.push_back({ x1, 0.0f, z1, c, c, c, a });
			vertices.push_back({ x0, 0.0f, z1, c, c, c, a });
		}
	}

	return vertices;
}

inline FVertexSimple LocalAxisVertices[6] = {
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }, // X축 시작점 (빨간색)
	{ 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }, // X축 끝점 (빨간색)
	{ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }, // Y축 시작점 (초록색)
	{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }, // Y축 끝점 (초록색)
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // Z축 시작점 (파란색)
	{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f } // Z축 끝점 (파란색)
};

inline FVertexSimple WorldAxisVertices[6] = {
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }, // X축 시작점 (빨간색)
	{ 100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }, // X축 끝점 (빨간색)
	{ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }, // Y축 시작점 (초록색)
	{ 0.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }, // Y축 끝점 (초록색)
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // Z축 시작점 (파란색)
	{ 0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f } // Z축 끝점 (파란색)
};

inline std::vector<FVertexSimple> CreateTranslateGizmoVertices()
{
	std::vector<FVertexSimple> vertices;

	// local +X 화살표를 X/Y/Z축으로 변환
	auto TransformAxis = [](const FVector3& p, int axis)
		{
			switch (axis)
			{
			case 0: // X
				return FVector3(p.x, p.y, p.z);

			case 1: // Y
				return FVector3(p.y, p.x, p.z);

			default: // Z
				return FVector3(p.y, p.z, p.x);
			}
		};

	auto AddVertex =
		[&](const FVector3& p, int axis, float r, float g, float b)
		{
			FVector3 transformed = TransformAxis(p, axis);

			vertices.push_back({
				transformed.x,
				transformed.y,
				transformed.z,
				r, g, b, 1.0f
				});
		};

	auto AddTriangle =
		[&](const FVector3& a,
			const FVector3& b,
			const FVector3& c,
			int axis,
			float r, float g, float bl)
		{
			AddVertex(a, axis, r, g, bl);
			AddVertex(b, axis, r, g, bl);
			AddVertex(c, axis, r, g, bl);
		};

	auto AddArrow =
		[&](int axis, float r, float g, float b)
		{
			const float h = GizmoGeometry::ShaftHalfWidth;

			// Shaft: local +X 방향 직육면체
			FVector3 p000(0.0f, -h, -h);
			FVector3 p001(0.0f, -h, h);
			FVector3 p010(0.0f, h, -h);
			FVector3 p011(0.0f, h, h);

			FVector3 p100(GizmoGeometry::ShaftLength, -h, -h);
			FVector3 p101(GizmoGeometry::ShaftLength, -h, h);
			FVector3 p110(GizmoGeometry::ShaftLength, h, -h);
			FVector3 p111(GizmoGeometry::ShaftLength, h, h);

			// 직육면체 6면
			AddTriangle(p000, p010, p011, axis, r, g, b);
			AddTriangle(p000, p011, p001, axis, r, g, b);

			AddTriangle(p100, p101, p111, axis, r, g, b);
			AddTriangle(p100, p111, p110, axis, r, g, b);

			AddTriangle(p000, p001, p101, axis, r, g, b);
			AddTriangle(p000, p101, p100, axis, r, g, b);

			AddTriangle(p010, p110, p111, axis, r, g, b);
			AddTriangle(p010, p111, p011, axis, r, g, b);

			AddTriangle(p000, p100, p110, axis, r, g, b);
			AddTriangle(p000, p110, p010, axis, r, g, b);

			AddTriangle(p001, p011, p111, axis, r, g, b);
			AddTriangle(p001, p111, p101, axis, r, g, b);

			// Arrow Head: 사각뿔
			const float ah = GizmoGeometry::TranslateHeadHalfWidth;

			FVector3 b0(GizmoGeometry::ShaftLength, -ah, -ah);
			FVector3 b1(GizmoGeometry::ShaftLength, ah, -ah);
			FVector3 b2(GizmoGeometry::ShaftLength, ah, ah);
			FVector3 b3(GizmoGeometry::ShaftLength, -ah, ah);

			FVector3 tip(GizmoGeometry::TranslateHeadLength, 0.0f, 0.0f);

			AddTriangle(tip, b0, b1, axis, r, g, b);
			AddTriangle(tip, b1, b2, axis, r, g, b);
			AddTriangle(tip, b2, b3, axis, r, g, b);
			AddTriangle(tip, b3, b0, axis, r, g, b);

			// 화살촉 밑면
			AddTriangle(b0, b3, b2, axis, r, g, b);
			AddTriangle(b0, b2, b1, axis, r, g, b);
		};

	AddArrow(0, 1.0f, 0.0f, 0.0f); // X Red
	AddArrow(1, 0.0f, 1.0f, 0.0f); // Y Green
	AddArrow(2, 0.0f, 0.0f, 1.0f); // Z Blue

	return vertices;
}

inline std::vector<FVertexSimple> CreateScaleGizmoVertices()
{
	std::vector<FVertexSimple> vertices;

	// +X 방향으로 만든 geometry를 X / Y / Z축으로 변환
	auto TransformAxis = [](const FVector3& p, int axis)
		{
			switch (axis)
			{
			case 0: // X
				return FVector3(p.x, p.y, p.z);

			case 1: // Y
				return FVector3(p.y, p.x, p.z);

			default: // Z
				return FVector3(p.y, p.z, p.x);
			}
		};

	auto AddVertex =
		[&](const FVector3& p, int axis, float r, float g, float b)
		{
			FVector3 t = TransformAxis(p, axis);

			vertices.push_back({
				t.x, t.y, t.z,
				r, g, b, 1.0f
				});
		};

	auto AddTriangle =
		[&](const FVector3& a,
			const FVector3& b,
			const FVector3& c,
			int axis,
			float r, float g, float bl)
		{
			AddVertex(a, axis, r, g, bl);
			AddVertex(b, axis, r, g, bl);
			AddVertex(c, axis, r, g, bl);
		};

	auto AddBox =
		[&](float x0, float x1,
			float halfWidth,
			int axis,
			float r, float g, float b)
		{
			FVector3 p000(x0, -halfWidth, -halfWidth);
			FVector3 p001(x0, -halfWidth, halfWidth);
			FVector3 p010(x0, halfWidth, -halfWidth);
			FVector3 p011(x0, halfWidth, halfWidth);

			FVector3 p100(x1, -halfWidth, -halfWidth);
			FVector3 p101(x1, -halfWidth, halfWidth);
			FVector3 p110(x1, halfWidth, -halfWidth);
			FVector3 p111(x1, halfWidth, halfWidth);

			// -X
			AddTriangle(p000, p010, p011, axis, r, g, b);
			AddTriangle(p000, p011, p001, axis, r, g, b);

			// +X
			AddTriangle(p100, p101, p111, axis, r, g, b);
			AddTriangle(p100, p111, p110, axis, r, g, b);

			// -Y
			AddTriangle(p000, p001, p101, axis, r, g, b);
			AddTriangle(p000, p101, p100, axis, r, g, b);

			// +Y
			AddTriangle(p010, p110, p111, axis, r, g, b);
			AddTriangle(p010, p111, p011, axis, r, g, b);

			// -Z
			AddTriangle(p000, p100, p110, axis, r, g, b);
			AddTriangle(p000, p110, p010, axis, r, g, b);

			// +Z
			AddTriangle(p001, p011, p111, axis, r, g, b);
			AddTriangle(p001, p111, p101, axis, r, g, b);
		};

	auto AddScaleHandle =
		[&](int axis, float r, float g, float b)
		{
			// shaft
			AddBox(
				0.0f,
				GizmoGeometry::ShaftLength,
				GizmoGeometry::ShaftHalfWidth,
				axis,
				r, g, b
			);

			// 끝의 cube
			AddBox(
				GizmoGeometry::ScaleHeadMin,
				GizmoGeometry::ScaleHeadMax,
				GizmoGeometry::ScaleHeadHalfWidth,
				axis,
				r, g, b
			);
		};

	AddScaleHandle(0, 1.0f, 0.0f, 0.0f); // X
	AddScaleHandle(1, 0.0f, 1.0f, 0.0f); // Y
	AddScaleHandle(2, 0.0f, 0.0f, 1.0f); // Z

	return vertices;
}

inline std::vector<FVertexSimple> CreateRotateGizmoVertices()
{
	std::vector<FVertexSimple> vertices;

	constexpr float TwoPI = 2.0f * PI;

	auto AddVertex =
		[&](const FVector3& p, float r, float g, float b)
		{
			vertices.push_back({
				p.x, p.y, p.z,
				r, g, b, 1.0f
				});
		};

	auto AddTriangle =
		[&](const FVector3& a,
			const FVector3& b,
			const FVector3& c,
			float r, float g, float bl)
		{
			AddVertex(a, r, g, bl);
			AddVertex(b, r, g, bl);
			AddVertex(c, r, g, bl);
		};

	auto GetRingPoint =
		[](float angle, float radius, int axis)
		{
			float c = cosf(angle) * radius;
			float s = sinf(angle) * radius;

			switch (axis)
			{
			case 0:
				// X축 중심 회전 → YZ 평면
				return FVector3(0.0f, c, s);

			case 1:
				// Y축 중심 회전 → XZ 평면
				return FVector3(c, 0.0f, s);

			default:
				// Z축 중심 회전 → XY 평면
				return FVector3(c, s, 0.0f);
			}
		};

	auto AddRing =
		[&](int axis, float r, float g, float b)
		{
			for (int i = 0; i < GizmoGeometry::RotateSegments; ++i)
			{
				float angle0 =
					TwoPI * static_cast<float>(i)
					/ GizmoGeometry::RotateSegments;

				float angle1 =
					TwoPI * static_cast<float>(i + 1)
					/ GizmoGeometry::RotateSegments;

				FVector3 outer0 = GetRingPoint(
					angle0,
					GizmoGeometry::RotateOuterRadius,
					axis
				);

				FVector3 outer1 = GetRingPoint(
					angle1,
					GizmoGeometry::RotateOuterRadius,
					axis
				);

				FVector3 inner0 = GetRingPoint(
					angle0,
					GizmoGeometry::RotateInnerRadius,
					axis
				);

				FVector3 inner1 = GetRingPoint(
					angle1,
					GizmoGeometry::RotateInnerRadius,
					axis
				);

				// 링 한 조각 = 삼각형 2개
				AddTriangle(
					outer0, outer1, inner1,
					r, g, b
				);

				AddTriangle(
					outer0, inner1, inner0,
					r, g, b
				);
			}
		};

	AddRing(0, 1.0f, 0.0f, 0.0f); // X = Red
	AddRing(1, 0.0f, 1.0f, 0.0f); // Y = Green
	AddRing(2, 0.0f, 0.0f, 1.0f); // Z = Blue

	return vertices;
}
#pragma once
