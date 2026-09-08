#pragma once

#include "FGizmo.h"

class UCamera;
class UPrimitiveComponent;

class FGizmoManipulator
{
public:
    void HandleMouseInput(bool allowWorldInput, EGizmoAxis hoveredAxis);
    void UpdateGizmoDrag(UCamera& Camera, const FGizmo& Gizmo, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode);

    EGizmoAxis GetActiveAxis() const { return ActiveAxis; }

private:
    void UpdateTranslateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, bool isLocalAxisMode);
    void UpdateRotateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode);
    void UpdateScaleDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive);

    FVector3 GetAxisDirection(EGizmoAxis Axis);
    float CalculateDragAmount(UCamera& Camera, const FVector3& WorldAxis);

    EGizmoAxis ActiveAxis = EGizmoAxis::None;
};