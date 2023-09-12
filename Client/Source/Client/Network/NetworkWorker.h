#pragma once

#include "CoreMinimal.h"
#include "Client.h"

class PacketSession;

struct CLIENT_API FPacketHeader
{
	FPacketHeader() : PacketSize(0), PacketID(0) { }

	FPacketHeader(uint16 PacketSize, uint16 PacketID)
		: PacketSize(PacketSize), PacketID(PacketID) { }

	friend FArchive& operator<<(FArchive& Ar, FPacketHeader& Header)
	{
		Ar << Header.PacketSize;
		Ar << Header.PacketID;
		return Ar;
	}

	uint16 PacketSize;
	uint16 PacketID;
};

/*-----------------
	 RecvWorker
-----------------*/

class CLIENT_API RecvWorker : public FRunnable
{
public:
	RecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session);
	~RecvWorker();

public:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	
private:
	bool ReceivePacket(TArray<uint8>& OutPacket);
	bool ReceiveDesiredBytes(uint8* Results, int32 Size);

protected:
	FRunnableThread* Thread;
	bool bRunning = true;
	
	FSocket* Socket;
	TWeakPtr<PacketSession> SessionRef;
};

/*-----------------
	 SendWorker
-----------------*/

class CLIENT_API SendWorker : public FRunnable
{
public:
	SendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session);
	~SendWorker();

public:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

public:
	bool SendPacket(SendBufferRef SendBuffer);
	
private:
	bool SendDesiredBytes(const uint8* Buffer, int32 Size);

protected:
	FRunnableThread* Thread;
	bool bRunning = true;

	FSocket* Socket;
	TWeakPtr<PacketSession> SessionRef;
};
