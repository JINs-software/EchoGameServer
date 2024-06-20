#pragma once
#include "CLanGroupServer.h"

#define TOTAL_AUTH_LIMITATION						1

class AuthThread : public CLanGroupThread
{
public:
	AuthThread(bool setTlsMemPool, size_t tlsMemPoolUnitCnt, size_t tlsMemPoolCapacity)
		: CLanGroupThread(setTlsMemPool, tlsMemPoolUnitCnt, tlsMemPoolCapacity) {}
private:
	void	ProcessAuth(SessionID sessionID, INT64 accountNo, char SessionKey[64]);
	bool	AuthQueryToRedis(INT64 accountNo, char SessionKey[64]);
	void	SendLoginResponse(SessionID sessionID, INT64 accountNo, BYTE status);

	// Redis ����(����) �� ��Ÿ ���� ������ �ʱ�ȭ �۾�
	virtual void OnStart();
	// �α��� ��û �޽��� ���� 
	virtual void OnMessage(SessionID sessionID, JBuffer& recvData);
	virtual void OnStop() {}
};

