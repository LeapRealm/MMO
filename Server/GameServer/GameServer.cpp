#include "pch.h"

#include <tchar.h>
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "Job.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		service->GetIocpCore()->Dispatch(10);
		ThreadManager::DistributeReservedJobs();
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ServerPacketHandler::Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); },
		100
	);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
		{
			DoWorkerJob(service);
		});
	}

	while (true)
	{
		Protocol::S_CHAT pkt;
		pkt.set_msg("Hello, Client, I'm Server");
		SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		GSessionManager.Broadcast(SendBuffer);
		this_thread::sleep_for(1s);
	}

	//DoWorkerJob(service);

	GThreadManager->Join();
}
