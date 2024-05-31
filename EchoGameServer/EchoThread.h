#pragma once
#include "CLanGroupServer.h"

#define ON_MESSAGE_BUFFERING

class EchoThread : public CLanGroupThread
{
public:
	EchoThread(bool setTlsMemPool, size_t tlsMemPoolUnitCnt, size_t tlsMemPoolCapacity)
		: CLanGroupThread(setTlsMemPool, tlsMemPoolUnitCnt, tlsMemPoolCapacity) {}
private:
	void ProcessEchoMessage(SessionID sessionID, INT64 accountNo, LONGLONG sendTick);

	// ���� ������ �ʱ�ȭ
	virtual void OnStart();
	// ���� �޽��� ���� ó��
	virtual void OnMessage(SessionID sessionID, JBuffer& recvData);
	virtual void OnStop() {}
};

