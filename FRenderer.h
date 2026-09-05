#pragma once

#include "FMatrix.h"
#include "UCamera.h"
#include "FVector3.h"
#include "URenderer.h"

class URenderer;
class UPrimitiveComponent;
struct ID3D11Buffer;

class FRenderer
{
public:
    FRenderer(URenderer& renderer);
    void Render(
        const UCamera& camera,
		const UPrimitiveComponent& primitive,
        ID3D11Buffer* vertexBuffer,
        UINT numVertices
    );

private:
    URenderer& Renderer;

	FMatrix CreateRotationZXY(const FVector3& rotation);
    FMatrix CreateModelMatrix(
        const FVector3& location,
        const FVector3& rotation,
        const FVector3& scale
    );

    FVector3 Right = FVector3(1.0f, 0.0f, 0.0f);
    FVector3 Up = FVector3(0.0f, 1.0f, 0.0f);
    FVector3 Forward = FVector3(0.0f, 0.0f, 1.0f);

    FMatrix CreateViewMatrix(const UCamera& camera);
	FMatrix CreateProjectionMatrix(const UCamera& camera);    
};