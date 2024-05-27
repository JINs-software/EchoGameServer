#pragma once
#include "CLanGroupServer.h"

class AuthThread : public CLanGroupThread
{
private:
	void	ProcessAuth(SessionID sessionID, INT64 accountNo, char SessionKey[64]);
	bool	AuthQueryToRedis(INT64 accountNo, char SessionKey[64]);
	void	SendLoginResponse(SessionID sessionID, INT64 accountNo, BYTE status);

	// Redis 연동(연결) 및 기타 인증 스레드 초기화 작업
	virtual void OnStart();
	// 로그인 요청 메시지 수신 
	virtual void OnRecv(SessionID sessionID, JBuffer& recvData);
	virtual void OnStop() {}
};

