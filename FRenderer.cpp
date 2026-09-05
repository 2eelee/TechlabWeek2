#include "FRenderer.h"
#include "URenderer.h"
#include "USceneComponent.h"

FRenderer::FRenderer(URenderer& renderer)
    : Renderer(renderer)
{
}

FMatrix FRenderer::CreateRotationZXY(const FVector3& rotation)
{
    FMatrix Rz = FMatrix::CreateRotationZ(rotation.z);
    FMatrix Rx = FMatrix::CreateRotationX(rotation.x);
    FMatrix Ry = FMatrix::CreateRotationY(rotation.y);

    return Rz * Rx * Ry;
}

// for each Object
FMatrix FRenderer::CreateModelMatrix(const FVector3& location, const FVector3& rotation, const FVector3& scale)
{
	FMatrix Scale = FMatrix::CreateScale(scale.x, scale.y, scale.z);
	FMatrix Rotation = CreateRotationZXY(rotation);
	FMatrix Translation = FMatrix::CreateTranslation(location.x, location.y, location.z);

    return Scale * Rotation * Translation;
}

// for each Camera
FMatrix FRenderer::CreateViewMatrix(const UCamera& camera)
{
    FMatrix Rotation = CreateRotationZXY(camera.Rotation);

    FVector3 rotatedRight = FMatrix::TransformDirection(Right, Rotation);
    FVector3 rotatedUp = FMatrix::TransformDirection(Up, Rotation);
    FVector3 rotatedForward = FMatrix::TransformDirection(Forward, Rotation);

    return FMatrix::CreateView(
        camera.Location,
        rotatedRight,
        rotatedUp,
        rotatedForward
    );
}

FMatrix FRenderer::CreateProjectionMatrix(const UCamera& camera)
{
    float width = Renderer.ViewportInfo.Width;
    float height = Renderer.ViewportInfo.Height > 0.0f
        ? Renderer.ViewportInfo.Height
        : 1.0f;

    float aspect = width / height;

    const float nearZ = 0.1f;
    const float farZ = 1000.0f;

    return FMatrix::CreateProjection(
        farZ,
        nearZ,
        camera.FOV,
        aspect
    );
}

void FRenderer::Render(const UCamera& camera, const UPrimitiveComponent& primitive, ID3D11Buffer* vertexBuffer, UINT numVertices)
{
    FMatrix Model = CreateModelMatrix(
        primitive.RelativeLocation,
        primitive.RelativeRotation,
        primitive.RelativeScale3D
    );

    FMatrix View = CreateViewMatrix(camera);

    FMatrix Proj = CreateProjectionMatrix(camera);

    FMatrix MVP = Model * View * Proj;

    Renderer.UpdateConstant(MVP);
    Renderer.RenderPrimitive(vertexBuffer, numVertices);
}