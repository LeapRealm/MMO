#include "NetworkWorker.h"

#include "PacketSession.h"
#include "Sockets.h"

/*-----------------
	 RecvWorker
-----------------*/

RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session)
	: Socket(Socket), SessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

RecvWorker::~RecvWorker()
{
	
}

bool RecvWorker::Init()
{
	LOG_SCREEN(TEXT("Recv Thread Init"));
	return true;
}

uint32 RecvWorker::Run()
{
	while (bRunning)
	{
		TArray<uint8> Packet;
		if (ReceivePacket(OUT Packet))
		{
			if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
				Session->RecvPacketQueue.Enqueue(Packet);
		}
	}
	
	return 0;
}

void RecvWorker::Stop()
{
	bRunning = false;
}

bool RecvWorker::ReceivePacket(TArray<uint8>& OutPacket)
{
	const int32 HeaderSize = sizeof(FPacketHeader);
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);

	if (ReceiveDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
		return false;

	FPacketHeader Header;
	{
		FMemoryReader Reader(HeaderBuffer);
		Reader << Header;
		LOG(TEXT("Recv PacketID : %d, PacketSize : %d"), Header.PacketID, Header.PacketSize);
	}

	OutPacket = HeaderBuffer;
	
	const int32 PayloadSize = Header.PacketSize - HeaderSize;
	if (PayloadSize <= 0)
		return true;
	
	OutPacket.AddZeroed(PayloadSize);
	
	if (ReceiveDesiredBytes(&OutPacket[HeaderSize], PayloadSize))
		return true;
	return false;
}

bool RecvWorker::ReceiveDesiredBytes(uint8* Results, int32 Size)
{
	uint32 PendingDataSize;
	if (Socket->HasPendingData(OUT PendingDataSize) == false || PendingDataSize <= 0)
		return false;

	int32 Offset = 0;

	while (Size > 0)
	{
		int32 NumRead = 0;
		Socket->Recv(Results + Offset, Size, OUT NumRead);

		if (NumRead <= 0)
			return false;

		Offset += NumRead;
		Size -= NumRead;
	}

	return true;
}

/*-----------------
	 SendWorker
-----------------*/

SendWorker::SendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session)
	: Socket(Socket), SessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

SendWorker::~SendWorker()
{
	
}

bool SendWorker::Init()
{
	LOG_SCREEN(TEXT("Send Thread Init"));
	return true;
}

uint32 SendWorker::Run()
{
	while (bRunning)
	{
		SendBufferRef SendBuffer;
		if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
		{
			if (Session->SendPacketQueue.Dequeue(OUT SendBuffer))
				SendPacket(SendBuffer);
		}
	}

	return 0;
}

void SendWorker::Stop()
{
	bRunning = false;
}

bool SendWorker::SendPacket(SendBufferRef SendBuffer)
{
	if (SendDesiredBytes(SendBuffer->Buffer(), SendBuffer->WriteSize()) == false)
		return false;
	return true;
}

bool SendWorker::SendDesiredBytes(const uint8* Buffer, int32 Size)
{
	while (Size > 0)
	{
		int32 BytesSent = 0;
		if (Socket->Send(Buffer, Size, OUT BytesSent) == false)
			return false;

		Size -= BytesSent;
		Buffer += BytesSent;
	}
	
	return true;
}
