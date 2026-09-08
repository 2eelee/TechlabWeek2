#pragma once

#include "FGizmo.h"

class UCamera;
class UPrimitiveComponent;

class FGizmoManipulator
{
public:
    void HandleMouseInput(bool allowWorldInput, EGizmoAxis hoveredAxis);
    void UpdateGizmoDrag(UCamera& Camera, const FGizmo& Gizmo, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight);

    EGizmoAxis GetActiveAxis() const { return ActiveAxis; }

private:
    void UpdateTranslateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive);
    void UpdateRotateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight);
    void UpdateScaleDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive);

    FVector3 GetAxisDirection(EGizmoAxis Axis);
    void GetRotationPlaneAxes(EGizmoAxis Axis, FVector3& DirA, FVector3& DirB);
    float CalculateDragAmount(UCamera& Camera, const FVector3& WorldAxis);

    EGizmoAxis ActiveAxis = EGizmoAxis::None;
};