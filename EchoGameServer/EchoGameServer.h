#pragma once
#include "CLanGroupServer.h"
#include "Configuration.h"

#include "AuthThread.h"
#include "EchoThread.h"

class EchoGameServer : public CLanGroupServer
{
public:
	//upServer(const char* serverIP, uint16 serverPort, DWORD numOfIocpConcurrentThrd, uint16 numOfWorkerThreads, uint16 maxOfConnections)
	EchoGameServer(const char* serverIP, uint16 serverPort, DWORD numOfIocpConcurrentThrd, uint16 numOfWorkerThreads, uint16 maxOfConnections, uint32 sessionSendBuffSize, uint32 sessionRecvBuffSize)
#if defined(ALLOC_BY_TLS_MEM_POOL)
		: CLanGroupServer(serverIP, serverPort, numOfIocpConcurrentThrd, numOfWorkerThreads, maxOfConnections) {
#else
		: CLanGroupServer(serverIP, serverPort, numOfIocpConcurrentThrd, numOfWorkerThreads, maxOfConnections, sessionSendBuffSize, sessionRecvBuffSize) {
#endif
		// 그룹 스레드 생성 및 셋팅
		CreateGroup(AUTH_SESSION_GROUP, new AuthThread(true, AUTH_GROUP_TLSMEMPOOL_UNIT_CNT, AUTH_GROUP_TLSMEMPOOL_CAPACITY), true);
		CreateGroup(ECHO_SESSION_GROUP_0, new EchoThread(true, ECHO_GROUP_TLSMEMPOOL_UNIT_CNT, ECHO_GROUP_TLSMEMPOOL_CAPACITY), false);
#if defined(MULTI_ECHO_SESSION_GROUP)
		CreateGroup(ECHO_SESSION_GROUP_1, new EchoThread(true, ECHO_GROUP_TLSMEMPOOL_UNIT_CNT, ECHO_GROUP_TLSMEMPOOL_CAPACITY));
		CreateGroup(ECHO_SESSION_GROUP_2, new EchoThread(true, ECHO_GROUP_TLSMEMPOOL_UNIT_CNT, ECHO_GROUP_TLSMEMPOOL_CAPACITY));
		CreateGroup(ECHO_SESSION_GROUP_3, new EchoThread(true, ECHO_GROUP_TLSMEMPOOL_UNIT_CNT, ECHO_GROUP_TLSMEMPOOL_CAPACITY));
#endif

	}
private:
	//virtual bool OnWorkerThreadCreate(HANDLE thHnd) { return true; };
	//virtual void OnWorkerThreadCreateDone() {}
	//virtual void OnWorkerThreadStart() {}
	//virtual void OnWorkerThreadEnd() {}

	// 클라이언트 접속 -> 인증 그룹에 세션 정보 전달
	virtual void OnClientJoin(SessionID sessionID);
	virtual void OnClientLeave(SessionID sessionID);
};

