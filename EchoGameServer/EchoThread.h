#pragma once
#include "CLanGroupServer.h"

class EchoThread : public CLanGroupThread
{
public:
	EchoThread(bool setTlsMemPool, size_t tlsMemPoolUnitCnt, size_t tlsMemPoolCapacity)
		: CLanGroupThread(setTlsMemPool, tlsMemPoolUnitCnt, tlsMemPoolCapacity) {}
private:
	void ProcessEchoMessage(SessionID sessionID, INT64 accountNo, LONGLONG sendTick);

	// 에코 스레드 초기화
	virtual void OnStart();
	// 에코 메시지 수신 처리
	virtual void OnRecv(SessionID sessionID, JBuffer& recvData);
	virtual void OnStop() {}
};

