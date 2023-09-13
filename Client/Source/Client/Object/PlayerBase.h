#pragma once

#include "CoreMinimal.h"
#include "CreatureBase.h"
#include "PlayerBase.generated.h"

UCLASS()
class CLIENT_API APlayerBase : public ACreatureBase
{
	GENERATED_BODY()

public:
	APlayerBase();
};
