#pragma once
#include "CLanGroupServer.h"
class EchoThread : public CLanGroupThread
{
private:
	void ProcessEchoMessage(SessionID sessionID, INT64 accountNo, LONGLONG sendTick);

	// ���� ������ �ʱ�ȭ
	virtual void OnStart() {}
	// ���� �޽��� ���� ó��
	virtual void OnRecv(SessionID sessionID, JBuffer& recvData);
	virtual void OnStop() {}
};

