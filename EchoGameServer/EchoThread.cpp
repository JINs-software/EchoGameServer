#include "EchoThread.h"
#include "CommonProtocol.h"

void EchoThread::ProcessEchoMessage(SessionID sessionID, INT64 accountNo, LONGLONG sendTick)
{
	JBuffer* resMsg = new JBuffer(sizeof(stMSG_HDR) + sizeof(stMSG_GAME_RES_ECHO));
	stMSG_HDR* hdr = resMsg->DirectReserve<stMSG_HDR>();
	hdr->code = dfPACKET_CODE;
	hdr->len = sizeof(stMSG_GAME_RES_ECHO);
	hdr->randKey = (BYTE)rand();

	*resMsg << (WORD)en_PACKET_CS_GAME_RES_ECHO;
	*resMsg << accountNo;
	*resMsg << sendTick;

	Encode(hdr->randKey, hdr->len, hdr->checkSum, resMsg->GetBufferPtr(sizeof(stMSG_HDR)));
	//if (!SendPacket(sessionID, resMsg)) {
	//	DebugBreak();
	//}
	// => EchoGameServer에서의 SendPacket 실패에 대해 추가적인 작업 없음(?)
	SendPacket(sessionID, resMsg);
}

void EchoThread::OnRecv(SessionID sessionID, JBuffer& recvData)
{
	// 에코 요청 메시지 수신
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

		stMSG_GAME_REQ_ECHO msg;
		recvData >> msg;
		if (msg.Type == en_PACKET_CS_GAME_REQ_ECHO) {
			ProcessEchoMessage(sessionID, msg.AccountoNo, msg.SendTick);
		}
		else {
			DebugBreak();
		}
	}

	if (recvData.GetUseSize() != 0) {
		DebugBreak();
	}
}
