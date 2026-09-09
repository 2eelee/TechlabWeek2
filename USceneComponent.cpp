#include "USceneManager.h"

void UPrimitiveComponent::Render(URenderer& renderer)
{
	renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (VertexBuffer && VertexCount > 0)
	{
		renderer.RenderPrimitive(VertexBuffer, VertexCount);
	}
}

FMatrix UPrimitiveComponent::GetModelMatrix() const
{
	FMatrix ScaleM = FMatrix::CreateScale(GetRelativeScale3D().x, GetRelativeScale3D().y, GetRelativeScale3D().z);
	FMatrix RotationM = GetRelativeRotationQuaternion().ToMatrix();
	FMatrix TranslationM = FMatrix::CreateTranslation(GetRelativeLocation().x, GetRelativeLocation().y, GetRelativeLocation().z);
	return ScaleM * RotationM * TranslationM;
}