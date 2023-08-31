#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
#include <atomic>
#include <mutex>
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "Listener.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameRoom.h"
#include "ServerPacketHandler.h"

int main()
{
	SocketUtils::Init();
	GRoom->Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[](){ return make_shared<GameSession>(); }, // TODO : SessionManager 등
		100);

	assert(service->Start());

	for (int32 i = 0; i < 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch(0);
					GRoom->Update();
				}
			});
	}

	


	GThreadManager->Join();

	// 윈속 종료
	SocketUtils::Clear();	
}