#include "AuthThread.h"
#include "Configuration.h"
#include "CommonProtocol.h"

void AuthThread::ProcessAuth(SessionID sessionID, INT64 accountNo, char sessionKey[64])
{
	// 레디스에 요청
	// 요청에 대한 반환에 따라
	if (AuthQueryToRedis(accountNo, sessionKey)) {
		// - 인증 성공
		// (1) EnterGroup(세션, 에코 그룹)
#if defined(MULTI_ECHO_SESSION_GROUP)
		GroupID gid;
		if (sessionID % 4 == 0) {
			gid = ECHO_SESSION_GROUP_0;
		}
		else if (sessionID % 4 == 1) {
			gid = ECHO_SESSION_GROUP_1;
		}
		else if (sessionID % 4 == 2) {
			gid = ECHO_SESSION_GROUP_2;
		}
		else if (sessionID % 4 == 3) {
			gid = ECHO_SESSION_GROUP_3;
		}
		ForwardSessionGroup(sessionID, gid);
#else
		ForwardSessionGroup(sessionID, ECHO_SESSION_GROUP_0);
#endif
		// (2) 성공 반환 메시지 전달
		SendLoginResponse(sessionID, accountNo, 1);
	}
	else {
		// - 인증 실패
		// (1) 실패 반환 메시지 전달
		SendLoginResponse(sessionID, accountNo, 0);
	}
}

bool AuthThread::AuthQueryToRedis(INT64 accountNo, char SessionKey[64])
{
	return true;
}

void AuthThread::SendLoginResponse(SessionID sessionID, INT64 accountNo, BYTE status)
{
#if defined(ALLOC_BY_TLS_MEM_POOL)
	JBuffer* resMsg = AllocSerialSendBuff(sizeof(stMSG_GAME_RES_LOGIN));
#else 
	JBuffer* resMsg = new JBuffer(sizeof(stMSG_HDR) + sizeof(stMSG_GAME_RES_LOGIN));
	stMSG_HDR* hdr = resMsg->DirectReserve<stMSG_HDR>();
	hdr->code = dfPACKET_CODE;
	hdr->len = sizeof(stMSG_GAME_RES_LOGIN);
	hdr->randKey = (BYTE)rand();
#endif

	*resMsg << (WORD)en_PACKET_CS_GAME_RES_LOGIN;
	*resMsg << status;
	*resMsg << accountNo;

	SendPacket(sessionID, resMsg);
}

void AuthThread::OnStart()
{
	// 레디스 연동
}

void AuthThread::OnMessage(SessionID sessionID, JBuffer& recvData)
{
	// 로그인 요청 메시지 수신
	LONG recvMsgCnt = 0;
	while (recvData.GetUseSize() >= sizeof(WORD)) {
		WORD type;
		recvData.Peek(&type);
		if (type != en_PACKET_CS_GAME_REQ_LOGIN) {
			DebugBreak();
		}
		else {
			stMSG_GAME_REQ_LOGIN msg;
			recvData >> msg;

			// 1. 버전 확인
			//	skip..

			// 2. 인증
			ProcessAuth(sessionID, msg.AccountNo, msg.SessionKey);
		}
		recvMsgCnt++;
	}
#if defined(CALCULATE_TRANSACTION_PER_SECOND)
	IncrementRecvTransaction(recvMsgCnt);
#endif

	if (recvData.GetUseSize() != 0) {
		DebugBreak();
	}
}
