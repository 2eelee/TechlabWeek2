#include "FGizmoManipulator.h"
#include "FGizmo.h"
#include "FMousePicker.h"
#include "UCamera.h"
#include "USceneComponent.h"
#include "InputManager.h"

#include <cmath>

// 마우스 입력으로 기즈모 드래그 시작/종료 축을 결정
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

// 현재 기즈모 모드에 맞는 Transform 드래그 처리를 호출
void FGizmoManipulator::UpdateGizmoDrag(UCamera& Camera, const FGizmo& Gizmo, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode)
{
    if (!SelectedPrimitive || ActiveAxis == EGizmoAxis::None) return;

    switch (Gizmo.GetGizmoMode())
    {
    case GizmoMode::Translate:
        UpdateTranslateDrag(Camera, SelectedPrimitive, ScreenWidth, ScreenHeight, isLocalAxisMode);
        break;
    case GizmoMode::Rotate:
        UpdateRotateDrag(Camera, SelectedPrimitive, ScreenWidth, ScreenHeight, isLocalAxisMode);
        break;
    case GizmoMode::Scale:
        UpdateScaleDrag(Camera, SelectedPrimitive, ScreenWidth, ScreenHeight);
        break;
    }
}


// Transform 조작

// 이전/현재 Mouse Ray가 가리키는 축 위치 차이만큼 오브젝트를 이동
void FGizmoManipulator::UpdateTranslateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode)
{
    FVector3 worldAxis = ResolveGizmoAxisDirection(ActiveAxis, SelectedPrimitive, isLocalAxisMode);
    FVector3 location = SelectedPrimitive->GetRelativeLocation();

    FRay previousRay{}, currentRay{};
    GetMouseDragRays(Camera, ScreenWidth, ScreenHeight, previousRay, currentRay);

    float previousT = 0.0f;
    float currentT = 0.0f;

    if (!GetClosestAxisParameter(previousRay, location, worldAxis, previousT)) return;
    if (!GetClosestAxisParameter(currentRay, location, worldAxis, currentT)) return;

    float amount = currentT - previousT;
    SelectedPrimitive->SetRelativeLocation(location + worldAxis * amount);
}

// 이전/현재 Mouse Ray와 회전 평면의 교점 방향 차이로 회전량을 구함
void FGizmoManipulator::UpdateRotateDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight, bool isLocalAxisMode)
{
    Quaternion CurrentQ = SelectedPrimitive->GetRelativeRotationQuaternion();

    FVector3 planeAxis = ResolveGizmoAxisDirection(ActiveAxis, SelectedPrimitive, isLocalAxisMode);
    FVector3 rotationAxis = GetBaseAxisDirection(ActiveAxis);
    FVector3 center = SelectedPrimitive->GetRelativeLocation();

    FRay previousRay{}, currentRay{};
    GetMouseDragRays(Camera, ScreenWidth, ScreenHeight, previousRay, currentRay);

    float deltaRad = 0.0f;
    if (!CalculateRotationDragDelta(previousRay, currentRay, center, planeAxis, deltaRad)) return;

    float halfDelta = deltaRad * 0.5f;
    float sinHalf = std::sin(halfDelta);
    float cosHalf = std::cos(halfDelta);

    Quaternion DeltaQ;
    DeltaQ.w = cosHalf;
    DeltaQ.x = rotationAxis.x * sinHalf;
    DeltaQ.y = rotationAxis.y * sinHalf;
    DeltaQ.z = rotationAxis.z * sinHalf;

    Quaternion NewQ;
    if (isLocalAxisMode)
        NewQ = CurrentQ * DeltaQ;
    else
        NewQ = DeltaQ * CurrentQ;

    NewQ.Normalize();
    SelectedPrimitive->SetRelativeRotation(NewQ);
}

// 이전/현재 Mouse Ray가 가리키는 Local 축 위치 차이만큼 Scale을 변경
void FGizmoManipulator::UpdateScaleDrag(UCamera& Camera, UPrimitiveComponent* SelectedPrimitive, float ScreenWidth, float ScreenHeight)
{
    // Scale은 항상 Local 축을 사용한다.
    FVector3 worldAxis = ResolveGizmoAxisDirection(ActiveAxis, SelectedPrimitive, true);
    FVector3 center = SelectedPrimitive->GetRelativeLocation();

    FRay previousRay{}, currentRay{};
    GetMouseDragRays(Camera, ScreenWidth, ScreenHeight, previousRay, currentRay);

    float previousT = 0.0f;
    float currentT = 0.0f;

    if (!GetClosestAxisParameter(previousRay, center, worldAxis, previousT)) return;
    if (!GetClosestAxisParameter(currentRay, center, worldAxis, currentT)) return;

    float amount = currentT - previousT;
    FVector3 scale = SelectedPrimitive->GetRelativeScale3D();

    switch (ActiveAxis)
    {
    case EGizmoAxis::X: scale.x += amount; break;
    case EGizmoAxis::Y: scale.y += amount; break;
    case EGizmoAxis::Z: scale.z += amount; break;
    case EGizmoAxis::None: return;
    }

    SelectedPrimitive->SetRelativeScale3D(scale);
}


// 축 변환

// 선택한 X/Y/Z 축의 기본 단위 방향벡터를 반환
FVector3 FGizmoManipulator::GetBaseAxisDirection(EGizmoAxis Axis)
{
    switch (Axis)
    {
    case EGizmoAxis::X: return { 1, 0, 0 };
    case EGizmoAxis::Y: return { 0, 1, 0 };
    case EGizmoAxis::Z: return { 0, 0, 1 };
    default: return { 0, 0, 0 };
    }
}

// World/Local 모드에 맞는 기즈모 축 방향을 반환
FVector3 FGizmoManipulator::ResolveGizmoAxisDirection(EGizmoAxis Axis, UPrimitiveComponent* SelectedPrimitive, bool isLocalAxisMode)
{
    FVector3 axisDir = GetBaseAxisDirection(Axis);
    if (!isLocalAxisMode) return axisDir;

    FMatrix RotationM = SelectedPrimitive->GetRelativeRotationQuaternion().ToMatrix();
    FVector4 rotated = FVector4{ axisDir.x, axisDir.y, axisDir.z, 0.0f } * RotationM;

    return { rotated.X, rotated.Y, rotated.Z };
}


// 마우스 드래그 계산

// 이전/현재 마우스 위치를 월드 공간의 Mouse Ray 두 개로 변환
void FGizmoManipulator::GetMouseDragRays(UCamera& Camera, float ScreenWidth, float ScreenHeight, FRay& PreviousRay, FRay& CurrentRay)
{
    FIntPoint currentMouseInt = InputManager::GetInstance().GetMousePosition();
    FIntPoint mouseDeltaInt = InputManager::GetInstance().GetMouseDelta();

    FVector2 currentMouse{
        static_cast<float>(currentMouseInt.X),
        static_cast<float>(currentMouseInt.Y)
    };

    FVector2 mouseDelta{
        static_cast<float>(mouseDeltaInt.X),
        static_cast<float>(mouseDeltaInt.Y)
    };

    FVector2 previousMouse = currentMouse - mouseDelta;

    FIntPoint previousMouseInt{
        static_cast<int>(previousMouse.X),
        static_cast<int>(previousMouse.Y)
    };

    PreviousRay = Camera.ScreenToRay(previousMouseInt, ScreenWidth, ScreenHeight);
    CurrentRay = Camera.ScreenToRay(currentMouseInt, ScreenWidth, ScreenHeight);
}

// 두 Mouse Ray의 회전 평면 교점으로부터 축 기준 회전각 변화량을 구함
bool FGizmoManipulator::CalculateRotationDragDelta(const FRay& PreviousRay, const FRay& CurrentRay, const FVector3& Center, const FVector3& AxisDirection, float& OutDeltaRad)
{
    FVector3 previousHit;
    FVector3 currentHit;

    if (!IntersectRayPlane(PreviousRay, Center, AxisDirection, previousHit))
        return false;

    if (!IntersectRayPlane(CurrentRay, Center, AxisDirection, currentHit))
        return false;

    FVector3 previousDir = previousHit - Center;
    FVector3 currentDir = currentHit - Center;

    if (previousDir.Length() < 0.000001f || currentDir.Length() < 0.000001f)
        return false;

    previousDir = previousDir.Normalize();
    currentDir = currentDir.Normalize();

    float dot = previousDir.Dot(currentDir);
    float cross = AxisDirection.Dot(previousDir.Cross(currentDir));

    OutDeltaRad = atan2f(cross, dot);

    return true;
}


// 교차 / 최근접 계산

// Mouse Ray와 평면의 교점을 구해 HitPoint로 반환
bool FGizmoManipulator::IntersectRayPlane(const FRay& Ray, const FVector3& PlanePoint, const FVector3& PlaneNormal, FVector3& HitPoint)
{
    const float EPSILON = 1.0e-6f;
    float denominator = Ray.Direction.Dot(PlaneNormal);

    if (std::abs(denominator) < EPSILON) return false;

    float t = (PlanePoint - Ray.Origin).Dot(PlaneNormal) / denominator;
    if (t < 0.0f) return false;

    HitPoint = Ray.Origin + Ray.Direction * t;
    return true;
}

// Mouse Ray에 가장 가까운 축 위 위치를 AxisT로 반환
bool FGizmoManipulator::GetClosestAxisParameter(const FRay& Ray, const FVector3& AxisOrigin, const FVector3& AxisDirection, float& OutAxisT)
{
    const float EPSILON = 1.0e-6f;

    FVector3 w = AxisOrigin - Ray.Origin;

    float a = AxisDirection.Dot(AxisDirection);
    float b = AxisDirection.Dot(Ray.Direction);
    float c = Ray.Direction.Dot(Ray.Direction);
    float d = AxisDirection.Dot(w);
    float e = Ray.Direction.Dot(w);

    float denominator = a * c - b * b;
    if (std::abs(denominator) < EPSILON)
        return false;

    float axisT = (b * e - c * d) / denominator;
    float rayT = (a * e - b * d) / denominator;

    if (rayT < 0.0f)
        return false;

    OutAxisT = axisT;
    return true;
}
