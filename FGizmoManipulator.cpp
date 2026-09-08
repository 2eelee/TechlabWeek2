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

void FGizmoManipulator::GetRotationPlaneAxes(EGizmoAxis Axis, FVector3& DirA, FVector3& DirB)
{
	switch (Axis)
	{
	case EGizmoAxis::X: DirA = { 0, 1, 0 }; DirB = { 0, 0, 1 }; break;
	case EGizmoAxis::Y: DirA = { 0, 0, 1 }; DirB = { 1, 0, 0 }; break;
	case EGizmoAxis::Z: DirA = { 1, 0, 0 }; DirB = { 0, 1, 0 }; break;
	case EGizmoAxis::None: return;
	}
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
	FMatrix CurrentR = FMatrix::CreateRotationX(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().y))
		* FMatrix::CreateRotationZ(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().z));
	
	// 마우스 현재 위치
	FIntPoint currentMouseInt = InputManager::GetInstance().GetMousePosition();
	FVector2 currentMouse{ static_cast<float>(currentMouseInt.X), static_cast<float>(currentMouseInt.Y) };

	// 마우스 델타값
	FIntPoint mouseDeltaInt = InputManager::GetInstance().GetMouseDelta();
	FVector2 mouseDelta{ static_cast<float>(mouseDeltaInt.X), static_cast<float>(mouseDeltaInt.Y) };

	// 마우스 과거 위치  = 현재 위치 - 델타값
	FVector2 previousMouse = currentMouse - mouseDelta;

	FVector3 Loc = SelectedPrimitive->GetRelativeLocation();
	FVector2 screenCenter = Camera.WorldToScreen(Loc, ScreenWidth, ScreenHeight);

	FVector3 dirA, dirB;
	GetRotationPlaneAxes(ActiveAxis, dirA, dirB);

	FVector2 screenA = Camera.WorldToScreen(Loc + dirA, ScreenWidth, ScreenHeight) - screenCenter;
	FVector2 screenB = Camera.WorldToScreen(Loc + dirB, ScreenWidth, ScreenHeight) - screenCenter;

	FVector2 previousDir = previousMouse - screenCenter;
	FVector2 currentDir = currentMouse - screenCenter;

	if (previousDir.Length() < 0.000001f || currentDir.Length() < 0.000001f) return;

	previousDir = previousDir.Normalize();
	currentDir = currentDir.Normalize();

	float dot = previousDir.Dot(currentDir);
	float cross = previousDir.X * currentDir.Y - previousDir.Y * currentDir.X;

	// atan2 → 화면에서 얼마나 돌았는지
	float deltaRad = atan2f(cross, dot);

	FMatrix DeltaR = FMatrix::Identity();
	switch (ActiveAxis)
	{
	case EGizmoAxis::X: DeltaR = FMatrix::CreateRotationX(deltaRad); break;
	case EGizmoAxis::Y: DeltaR = FMatrix::CreateRotationY(deltaRad); break;
	case EGizmoAxis::Z: DeltaR = FMatrix::CreateRotationZ(deltaRad); break;
	case EGizmoAxis::None: return;
	}

	FMatrix NewR = DeltaR * CurrentR;

	float radX, radY, radZ;
	if (fabsf(NewR.m[0][2]) < 0.9999f)
	{
		radY = asinf(std::clamp(-NewR.m[0][2], -1.0f, 1.0f));
		radZ = atan2f(NewR.m[0][1], NewR.m[0][0]);
		radX = atan2f(NewR.m[1][2], NewR.m[2][2]);
	}
	else
	{
		// 짐벌락 특이점 예외 처리
		radY = (NewR.m[0][2] < 0) ? (3.14159265f * 0.5f) : (-3.14159265f * 0.5f);
		radZ = 0.0f;
		radX = atan2f(-NewR.m[2][1], NewR.m[1][1]);
	}
	SelectedPrimitive->SetRelativeRotation(FVector3(
		RadiansToDegrees(radX),
		RadiansToDegrees(radY),
		RadiansToDegrees(radZ)
	));
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
