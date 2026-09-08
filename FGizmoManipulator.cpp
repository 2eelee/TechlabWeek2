#include "FGizmoManipulator.h"
#include "FGizmo.h"
#include "FMousePicker.h"
#include "UCamera.h"
#include "InputManager.h"

void FGizmoManipulator::HandleMouseInput(bool allowWorldInput, EGizmoAxis hoveredAxis)
{
	if (InputManager::GetInstance().GetMouseButtonUp(MouseButton::LEFT))
	{
		ActiveAxis = EGizmoAxis::None;
	}

	if (!allowWorldInput) return;

	if (InputManager::GetInstance().GetMouseButtonDown(MouseButton::LEFT) && hoveredAxis != EGizmoAxis::None)
	{
		ActiveAxis = hoveredAxis;
	}
}

void FGizmoManipulator::UpdateGizmoDrag(UCamera& Camera, const FGizmo& Gizmo, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight)
{
	if (!SelectedPrimitive || ActiveAxis == EGizmoAxis::None) return;

	switch (Gizmo.GetGizmoMode())
	{
	case GizmoMode::Translate: UpdateTranslateDrag(Camera, SelectedPrimitive); break;
	case GizmoMode::Rotate:    UpdateRotateDrag(Camera, SelectedPrimitive, ScreenWidth, ScreenHeight); break;
	case GizmoMode::Scale:     UpdateScaleDrag(Camera, SelectedPrimitive); break;
	}
}

FVector3 FGizmoManipulator::GetAxisDirection(EGizmoAxis Axis)
{
	FVector3 axisDir;

	switch (Axis)
	{
	case EGizmoAxis::X: axisDir = { 1, 0, 0 }; break;
	case EGizmoAxis::Y: axisDir = { 0, 1, 0 }; break;
	case EGizmoAxis::Z: axisDir = { 0, 0, 1 }; break;
	case EGizmoAxis::None: return { 0, 0, 0 };
	}
	return axisDir;
}

float FGizmoManipulator::CalculateDragAmount(UCamera& Camera, const FVector3& WorldAxis)
{
	FIntPoint mouseDeltaInt = InputManager::GetInstance().GetMouseDelta();
	FVector2 mouseDelta{ static_cast<float>(mouseDeltaInt.X), static_cast<float>(mouseDeltaInt.Y) };

	FVector2 screenAxis{ WorldAxis.Dot(Camera.GetRightVector()), -WorldAxis.Dot(Camera.GetUPVector()) };
	if (screenAxis.Length() < 0.000001f) return 0.0f;

	FVector2 screenAxisNormalized = screenAxis.Normalize();

	float sensitivity = 0.01f;
	float amount = mouseDelta.Dot(screenAxisNormalized) * sensitivity;

	return amount;
}

void FGizmoManipulator::UpdateTranslateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive)
{
	FVector3 axisDir = GetAxisDirection(ActiveAxis);
	float amount = CalculateDragAmount(Camera, axisDir);

	FVector3 Loc = SelectedPrimitive->GetRelativeLocation();
	Loc += axisDir * amount;
	SelectedPrimitive->SetRelativeLocation(Loc);
}

void FGizmoManipulator::UpdateRotateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight)
{
	FVector3 axisDir = GetAxisDirection(ActiveAxis);
	FVector3 objectPos = SelectedPrimitive->GetRelativeLocation();
	FVector3 cameraPos = Camera.GetRelativeLocation();

	FVector3 objectToCamera = cameraPos - objectPos;

	float side = axisDir.Dot(objectToCamera);

	FIntPoint currentMouseInt = InputManager::GetInstance().GetMousePosition();
	FVector2 currentMouse{ static_cast<float>(currentMouseInt.X), static_cast<float>(currentMouseInt.Y) };

	FIntPoint mouseDeltaInt = InputManager::GetInstance().GetMouseDelta();
	FVector2 mouseDelta{ static_cast<float>(mouseDeltaInt.X), static_cast<float>(mouseDeltaInt.Y) };

	FVector2 previousMouse = currentMouse - mouseDelta;

	FVector3 Loc = SelectedPrimitive->GetRelativeLocation();
	FVector2 screenCenter = Camera.WorldToScreen(Loc, ScreenWidth, ScreenHeight);

	FVector2 previousDir = previousMouse - screenCenter;
	FVector2 currentDir = currentMouse - screenCenter;

	previousDir = previousDir.Normalize();
	currentDir = currentDir.Normalize();

	float dot = previousDir.Dot(currentDir);
	float cross = previousDir.X * currentDir.Y - previousDir.Y * currentDir.X;

	float angleDegree = RadiansToDegrees(atan2f(cross, dot));

	float sign = side > 0.0f ? 1.0f : -1.0f;
	angleDegree *= sign;

	FVector3 Rotation = SelectedPrimitive->GetRelativeRotation();

	switch (ActiveAxis)
	{
		case EGizmoAxis::X: Rotation.x += angleDegree; break;
		case EGizmoAxis::Y: Rotation.y += angleDegree; break;
		case EGizmoAxis::Z: Rotation.z += angleDegree; break;
		case EGizmoAxis::None: return;
	}
	SelectedPrimitive->SetRelativeRotation(Rotation);
}

void FGizmoManipulator::UpdateScaleDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive)
{
	FVector3 axisDir = GetAxisDirection(ActiveAxis);

	FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().y))
		* FMatrix::CreateRotationZ(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().z));

	FVector4 rotated = FVector4{ axisDir.x, axisDir.y, axisDir.z, 0.0f } * RotationM;

	FVector3 worldAxis{ rotated.X, rotated.Y, rotated.Z };
	float amount = CalculateDragAmount(Camera, worldAxis);

	FVector3 Scale = SelectedPrimitive->GetRelativeScale3D();
	switch (ActiveAxis)
	{
		case EGizmoAxis::X: Scale.x += amount; break;
		case EGizmoAxis::Y: Scale.y += amount; break;
		case EGizmoAxis::Z: Scale.z += amount; break;
		case EGizmoAxis::None: return;
	}
	SelectedPrimitive->SetRelativeScale3D(Scale);
}
