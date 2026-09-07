#pragma once

#include "UCamera.h"
#include <algorithm>
#include "FMatrix.h"
#include "InputManager.h"
#include "ImGui/imgui.h"

FVector3 UCamera::GetForwardVector()
{
	float pitchrad = DegreesToRadians(this->GetRelativeRotation().x);
	float yawrad = DegreesToRadians(this->GetRelativeRotation().y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[2][0], R.m[2][1], R.m[2][2]);
}

FVector3 UCamera::GetRightVector()
{
	float pitchrad = DegreesToRadians(this->GetRelativeRotation().x);
	float yawrad = DegreesToRadians(this->GetRelativeRotation().y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[0][0], R.m[0][1], R.m[0][2]);
}

FVector3 UCamera::GetUPVector()
{
	float pitchrad = DegreesToRadians(this->GetRelativeRotation().x);
	float yawrad = DegreesToRadians(this->GetRelativeRotation().y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[1][0], R.m[1][1], R.m[1][2]);
}

void UCamera::SetOrthoWidth(float width)
{
	orthowidth = width;
}

void UCamera::AddPitch(float deltaAngle)
{
	FVector3 rot = GetRelativeRotation();
	rot.x = std::clamp(rot.x + deltaAngle, -89.9f, 89.9f);
	SetRelativeRotation(rot);
}

void UCamera::AddYaw(float deltaAngle)
{
	FVector3 rot = GetRelativeRotation();
	rot.y += deltaAngle;
	SetRelativeRotation(rot);
}

void UCamera::CamMove(float deltaTime)
{
	float speed = 10.0f;
	float moveDist = speed * deltaTime;

	if (!(ImGui::GetIO().WantCaptureKeyboard)) {
		if (InputManager::GetInstance().GetKey('W'))
		{
			FVector3 Loc = this->GetRelativeLocation();
			this->SetRelativeLocation(Loc + this->GetForwardVector() * moveDist);
		}
		if (InputManager::GetInstance().GetKey('S'))
		{
			FVector3 Loc = this->GetRelativeLocation();
			this->SetRelativeLocation(Loc - (this->GetForwardVector() * moveDist));
		}
		if (InputManager::GetInstance().GetKey('D'))
		{
			FVector3 Loc = this->GetRelativeLocation();
			this->SetRelativeLocation(Loc + this->GetRightVector() * moveDist);
		}
		if (InputManager::GetInstance().GetKey('A'))
		{
			FVector3 Loc = this->GetRelativeLocation();
			this->SetRelativeLocation(Loc - (this->GetRightVector() * moveDist));
		}
		if (InputManager::GetInstance().GetKey('Q'))
		{
			FVector3 Loc = this->GetRelativeLocation();
			this->SetRelativeLocation(Loc - (this->GetUPVector() * moveDist));
		}
		if (InputManager::GetInstance().GetKey('E'))
		{
			FVector3 Loc = this->GetRelativeLocation();
			this->SetRelativeLocation(Loc + (this->GetUPVector() * moveDist));
		}



		if (InputManager::GetInstance().GetMouseButton(MouseButton::RIGHT))
		{
			FIntPoint delta = InputManager::GetInstance().GetMouseDelta();

			const float sensitivity = 0.1f;

			AddYaw(delta.X * sensitivity);
			AddPitch(delta.Y * sensitivity);
		}

		float wheel = InputManager::GetInstance().GetMouseWheelDelta();

		if (othogonalEnable)
		{
			orthowidth *= std::pow(0.9f, wheel);
			orthowidth = std::clamp(orthowidth, 1.0f, 10000.0f);
		}
		else
		{
			FVector3 loc = GetRelativeLocation();
			loc += GetForwardVector() * wheel * 2.0f;
			SetRelativeLocation(loc);
		}
	}
}

FRay UCamera::ScreenToRay(FIntPoint ScreenPosition, float ScreenWidth, float ScreenHeight)
{
	FVector2 normalizedPoint = MathUtils::ScreenToNDC(ScreenPosition, ScreenWidth, ScreenHeight);
	FVector4 nearNormalized = { normalizedPoint.X, normalizedPoint.Y, 0.0f, 1.0f };
	FVector4 farNormalized = { normalizedPoint.X, normalizedPoint.Y, 1.0f, 1.0f };

	FMatrix InverseProj;
	if (othogonalEnable)
		InverseProj = FMatrix::CreateOrthogonalProjectionInverse(FarZ, NearZ, 20.0f, 20.0f);
	else InverseProj = FMatrix::CreateProjectionInverse(ScreenWidth / ScreenHeight, DegreesToRadians(FovAngle), FarZ, NearZ);

	FMatrix InverseView = FMatrix::CreateView(GetRelativeLocation(), GetRightVector(), GetUPVector(), GetForwardVector()).Inverse();
	FVector4 deprojectedFar = (farNormalized * InverseProj * InverseView).DivideByW();
	FVector4 deprojectedNear = (nearNormalized * InverseProj * InverseView).DivideByW();
	FVector3 rayOrigin = { deprojectedNear.X, deprojectedNear.Y, deprojectedNear.Z };
	FVector3 rayEnd = { deprojectedFar.X, deprojectedFar.Y, deprojectedFar.Z };
	FVector3 rayDirection = (rayEnd - rayOrigin).Normalize();

	return FRay{ rayOrigin, rayDirection };
}