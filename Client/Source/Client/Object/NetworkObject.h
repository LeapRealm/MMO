#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NetworkObject.generated.h"

UINTERFACE(MinimalAPI)
class UNetworkObject : public UInterface
{
	GENERATED_BODY()
};

class CLIENT_API INetworkObject
{
	GENERATED_BODY()

public:
	uint64 ID;
};
