#pragma once

#include "CoreMinimal.h"
#include "NetworkObject.h"
#include "GameFramework/Pawn.h"
#include "CreatureBase.generated.h"

UCLASS()
class CLIENT_API ACreatureBase : public APawn, public INetworkObject
{
	GENERATED_BODY()

public:
	ACreatureBase();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
