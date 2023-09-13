#include "PlayerBase.h"

APlayerBase::APlayerBase()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (SM_Cylinder.Succeeded())
		StaticMeshComponent->SetStaticMesh(SM_Cylinder.Object);
}
