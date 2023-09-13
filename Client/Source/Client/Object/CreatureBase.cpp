#include "CreatureBase.h"

ACreatureBase::ACreatureBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
}

void ACreatureBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACreatureBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
