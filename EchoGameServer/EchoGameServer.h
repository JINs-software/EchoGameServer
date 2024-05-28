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
		// �׷� ������ ���� �� ����
		CreateGroup(AUTH_SESSION_GROUP, new AuthThread());
		CreateGroup(ECHO_SESSION_GROUP, new EchoThread());
	}
private:
	//virtual bool OnWorkerThreadCreate(HANDLE thHnd) { return true; };
	//virtual void OnWorkerThreadCreateDone() {}
	//virtual void OnWorkerThreadStart() {}
	//virtual void OnWorkerThreadEnd() {}

	// Ŭ���̾�Ʈ ���� -> ���� �׷쿡 ���� ���� ����
	virtual void OnClientJoin(SessionID sessionID);

	virtual void OnClientLeave(SessionID sessionID);

	virtual UINT RecvData(JBuffer& recvBuff, JBuffer& dest);
};

