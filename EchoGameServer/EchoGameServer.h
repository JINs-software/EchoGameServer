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
		: CLanGroupServer(serverIP, serverPort, numOfIocpConcurrentThrd, numOfWorkerThreads, maxOfConnections, sessionSendBuffSize, sessionRecvBuffSize) {
		// 그룹 스레드 생성 및 셋팅
		CreateGroup(AUTH_SESSION_GROUP, new AuthThread());
		CreateGroup(ECHO_SESSION_GROUP, new EchoThread());
	}
private:
	//virtual bool OnWorkerThreadCreate(HANDLE thHnd) { return true; };
	//virtual void OnWorkerThreadCreateDone() {}
	//virtual void OnWorkerThreadStart() {}
	//virtual void OnWorkerThreadEnd() {}

	// 클라이언트 접속 -> 인증 그룹에 세션 정보 전달
	virtual void OnClientJoin(SessionID sessionID);

	virtual void OnClientLeave(SessionID sessionID);

	virtual UINT RecvData(JBuffer& recvBuff, JBuffer& dest);
};

