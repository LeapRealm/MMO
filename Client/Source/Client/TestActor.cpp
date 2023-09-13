#include "TestActor.h"

#include "Client.h"
#include "ClientGameInstance.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"

ATestActor::ATestActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATestActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Protocol::C_CHAT Msg;
	Msg.set_msg("Hello Server, I'm Client");
	SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Msg);
	Cast<UClientGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
}
