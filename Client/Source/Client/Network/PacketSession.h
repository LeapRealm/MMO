#pragma once

#include "CoreMinimal.h"
#include "Client.h"

class RecvWorker;
class SendWorker;

class CLIENT_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession(FSocket* Socket);
	~PacketSession();

public:
	void Run();

	void HandleRecvPackets();
	void SendPacket(SendBufferRef SendBuffer);
	
	void Disconnect();

public:
	FSocket* Socket;

	TSharedPtr<RecvWorker> RecvWorkerThread;
	TSharedPtr<SendWorker> SendWorkerThread;
	
	TQueue<TArray<uint8>> RecvPacketQueue;
	TQueue<SendBufferRef> SendPacketQueue;
};
