#include "ClientGameInstance.h"

#include "Client.h"
#include "PacketSession.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"

void UClientGameInstance::ConnectToGameServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address IPv4Address;
	FIPv4Address::Parse(IPAddress, OUT IPv4Address);
	
	TSharedPtr<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(IPv4Address.Value);
	InternetAddr->SetPort(Port);

	LOG_SCREEN(TEXT("Connecting To Server..."));

	bool bConnected = Socket->Connect(*InternetAddr);
	if (bConnected)
	{
		LOG_SCREEN(TEXT("Connection Success"));
		
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();
	}
	else
	{
		LOG_SCREEN(TEXT("Connection Failed"));
	}
}

void UClientGameInstance::DisconnectFromGameServer()
{
	if (Socket)
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}

	if (GameServerSession)
	{
		GameServerSession->Disconnect();
		GameServerSession = nullptr;
	}
}

void UClientGameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void UClientGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}
