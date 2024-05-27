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
		ForwardSessionGroup(sessionID, ECHO_SESSION_GROUP);
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
	JBuffer* resMsg = new JBuffer(sizeof(stMSG_HDR) + sizeof(stMSG_GAME_RES_LOGIN));
	stMSG_HDR* hdr = resMsg->DirectReserve<stMSG_HDR>();
	hdr->code = dfPACKET_CODE;
	hdr->len = sizeof(stMSG_GAME_RES_LOGIN);
	hdr->randKey = (BYTE)rand();

	*resMsg << (WORD)en_PACKET_CS_GAME_RES_LOGIN;
	*resMsg << status;
	*resMsg << accountNo;

	Encode(hdr->randKey, hdr->len, hdr->checkSum, resMsg->GetBufferPtr(sizeof(stMSG_HDR)));
	if (!SendPacket(sessionID, resMsg)) {
		DebugBreak();
	}
}

void AuthThread::OnStart()
{
	// 레디스 연동
}

void AuthThread::OnRecv(SessionID sessionID, JBuffer& recvData)
{
	// 로그인 요청 메시지 수신
	while (recvData.GetUseSize() >= sizeof(stMSG_HDR)) {
		stMSG_HDR msgHdr;
		recvData.Peek(&msgHdr);
		if (msgHdr.code != dfPACKET_CODE) {
			// 코드 불일치
			// 연결 강제 종료!
			DebugBreak();
			break;
		}
		if (recvData.GetUseSize() < sizeof(stMSG_HDR) + msgHdr.len) {
			// 메시지 미완성
			//DebugBreak();
			// => 메모리 풀 방식에서 스마트 포인터 방식 변경 시 미환성 메시지 수신 발생..
			break;
		}

		recvData >> msgHdr;
		if (!Decode(msgHdr.randKey, msgHdr.len, msgHdr.checkSum, recvData.GetDequeueBufferPtr())) {
			DebugBreak();
			// 연결 강제 종료?
		}

		stMSG_GAME_REQ_LOGIN msg;
		recvData >> msg;
		if (msg.Type == en_PACKET_CS_GAME_REQ_LOGIN) {
			// 1. 버전 확인
			//	skip..

			// 2. 인증
			ProcessAuth(sessionID, msg.AccountNo, msg.SessionKey);
		}
		else {
			DebugBreak();
		}
	}

	if (recvData.GetUseSize() != 0) {
		DebugBreak();
	}
}
