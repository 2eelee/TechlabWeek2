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

void FGizmoManipulator::UpdateGizmoDrag(UCamera& Camera, const FGizmo& Gizmo, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode)
{
	if (!SelectedPrimitive || ActiveAxis == EGizmoAxis::None) return;

	switch (Gizmo.GetGizmoMode())
	{
	case GizmoMode::Translate: 
		UpdateTranslateDrag(Camera, SelectedPrimitive, isLocalAxisMode);
		break;
	case GizmoMode::Rotate:    
		UpdateRotateDrag(Camera, SelectedPrimitive, ScreenWidth, ScreenHeight, isLocalAxisMode);
		break;
	case GizmoMode::Scale:     
		UpdateScaleDrag(Camera, SelectedPrimitive); 
		break;
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

void FGizmoManipulator::UpdateTranslateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, bool isLocalAxisMode)
{
	FVector3 axisDir = GetAxisDirection(ActiveAxis);
	FVector3 worldAxis = axisDir;

	if (isLocalAxisMode)
	{ 
		FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().x))
			* FMatrix::CreateRotationY(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().y))
			* FMatrix::CreateRotationZ(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().z));

		FVector4 rotated = FVector4{ axisDir.x, axisDir.y, axisDir.z, 0.0f } * RotationM;
		worldAxis = FVector3{ rotated.X, rotated.Y, rotated.Z };
	}

	float amount = CalculateDragAmount(Camera, worldAxis);
	FVector3 Loc = SelectedPrimitive->GetRelativeLocation();
	Loc += worldAxis * amount;
	SelectedPrimitive->SetRelativeLocation(Loc);
}

void FGizmoManipulator::UpdateRotateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode)
{
	FMatrix CurrentR = FMatrix::CreateRotationX(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().y))
		* FMatrix::CreateRotationZ(DegreesToRadians(SelectedPrimitive->GetRelativeRotation().z));
	
	FVector3 axisDir = GetAxisDirection(ActiveAxis);
	FVector3 objectPos = SelectedPrimitive->GetRelativeLocation();
	FVector3 cameraPos = Camera.GetRelativeLocation();

	FVector3 objectToCamera = cameraPos - objectPos;

	FIntPoint currentMouseInt = InputManager::GetInstance().GetMousePosition();
	FVector2 currentMouse{ static_cast<float>(currentMouseInt.X), static_cast<float>(currentMouseInt.Y) };

	FIntPoint mouseDeltaInt = InputManager::GetInstance().GetMouseDelta();
	FVector2 mouseDelta{ static_cast<float>(mouseDeltaInt.X), static_cast<float>(mouseDeltaInt.Y) };

	FVector2 previousMouse = currentMouse - mouseDelta;
	FIntPoint previousMouseInt{ static_cast<int>(previousMouse.X), static_cast<int>(previousMouse.Y) };

	FVector3 center = SelectedPrimitive->GetRelativeLocation();

	FRay previousRay = Camera.ScreenToRay(previousMouseInt, ScreenWidth, ScreenHeight);
	FRay currentRay = Camera.ScreenToRay(currentMouseInt, ScreenWidth, ScreenHeight);

	FVector3 previousHit, currentHit;

	if (!IntersectPlane(previousRay, center, axisDir, previousHit))
		return;
	if (!IntersectPlane(currentRay, center, axisDir, currentHit))
		return;

	FVector3 previousDir = previousHit - center;
	FVector3 currentDir = currentHit - center;

	if (previousDir.Length() < 0.000001f || currentDir.Length() < 0.000001f) 
		return;

	previousDir = previousDir.Normalize();
	currentDir = currentDir.Normalize();

	float dot = previousDir.Dot(currentDir);
	float cross = axisDir.Dot(previousDir.Cross(currentDir));

	float deltaRad = atan2f(cross, dot);

	FMatrix DeltaR = FMatrix::Identity();
	switch (ActiveAxis)
	{
	case EGizmoAxis::X: DeltaR = FMatrix::CreateRotationX(deltaRad); break;
	case EGizmoAxis::Y: DeltaR = FMatrix::CreateRotationY(deltaRad); break;
	case EGizmoAxis::Z: DeltaR = FMatrix::CreateRotationZ(-deltaRad); break;
	case EGizmoAxis::None: return;
	}

	FMatrix NewR;
	if (isLocalAxisMode)
		NewR = DeltaR * CurrentR;
	else
		NewR = CurrentR * DeltaR;

	float radX, radY, radZ;
	if (fabsf(NewR.m[0][2]) < 0.9999f)
	{
		radY = asinf(std::clamp(-NewR.m[0][2], -1.0f, 1.0f));
		radZ = atan2f(-NewR.m[0][1], NewR.m[0][0]);
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

bool FGizmoManipulator::IntersectPlane(const FRay& ray, const FVector3& planePoint, const FVector3& planeNormal, FVector3& hitPoint)
{
	float D = ray.Direction.Dot(planeNormal);

	const float EPSILON = 1.0e-6f;
	if (std::abs(D) < EPSILON) return false;

	float t = (planePoint - ray.Origin).Dot(planeNormal) / D;

	if (t < 0.0f) return false;

	hitPoint = ray.Origin + ray.Direction * t;
	return true;
}