#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCustom, Log, All);

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define LOG_CALLINFO(Verbosity) UE_LOG(LogCustom, Verbosity, TEXT("%s"), *CALLINFO)
#define LOG(Format, ...) UE_LOG(LogCustom, Display, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG_WARNING(Format, ...) UE_LOG(LogCustom, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG_ERROR(Format, ...) UE_LOG(LogCustom, Error, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN_COLOR(Color ,Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, FString::Printf(Format, ##__VA_ARGS__))

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

/*-----------------
	 SendBuffer
-----------------*/

class SendBuffer : public TSharedFromThis<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

public:
	BYTE* Buffer()		{ return _buffer.GetData(); }
	int32 WriteSize()	{ return _writeSize; }
	int32 Capacity()	{ return static_cast<int32>(_buffer.Num()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);

private:
	TArray<BYTE>	_buffer;
	int32			_writeSize = 0;
};

#define USING_SHARED_PTR(name) using name##Ref = TSharedPtr<class name>;
USING_SHARED_PTR(Session);
USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(SendBuffer);

#define SEND_PACKET(Pkt)															\
	SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Pkt);			\
	Cast<UClientGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
