#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestActor.generated.h"

UCLASS()
class CLIENT_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestActor();

public:	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
