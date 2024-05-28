#pragma once
#include "CLanGroupServer.h"

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
	virtual void OnRecv(SessionID sessionID, JBuffer& recvData);
	virtual void OnStop() {}
};

