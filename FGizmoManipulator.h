#pragma once

#include "FGizmo.h"

class UCamera;
class UPrimitiveComponent;
struct FRay;

class FGizmoManipulator
{
public:
    void HandleMouseInput(bool allowWorldInput, EGizmoAxis hoveredAxis);
    void UpdateGizmoDrag(UCamera& Camera, const FGizmo& Gizmo, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode);

    EGizmoAxis GetActiveAxis() const { return ActiveAxis; }

private:
    // Transform 조작
    void UpdateTranslateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode);
    void UpdateRotateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode);
    void UpdateScaleDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight);

    // 축 / 회전 변환
    FVector3 GetBaseAxisDirection(EGizmoAxis Axis);
    FVector3 ResolveGizmoAxisDirection(EGizmoAxis Axis, UPrimitiveComponent* SelectedPrimitive, bool isLocalAxisMode);

    // 마우스 드래그 계산
    void GetMouseDragRays(UCamera& Camera, float ScreenWidth, float ScreenHeight, FRay& PreviousRay, FRay& CurrentRay);
    bool CalculateRotationDragDelta(const FRay& PreviousRay, const FRay& CurrentRay, const FVector3& Center, const FVector3& AxisDirection, float& OutDeltaRad);

    // 교차 / 최근접 계산
    bool IntersectRayPlane(const FRay& Ray, const FVector3& PlanePoint, const FVector3& PlaneNormal, FVector3& HitPoint);
    bool GetClosestAxisParameter(const FRay& Ray, const FVector3& AxisOrigin, const FVector3& AxisDirection, float& OutAxisT);

    EGizmoAxis ActiveAxis = EGizmoAxis::None;
};