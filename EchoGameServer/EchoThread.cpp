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
	UINT bufferOffset = 0;
	while (recvData.GetUseSize() > bufferOffset) {
		stMSG_HDR* hdr = (stMSG_HDR*)recvData.GetBufferPtr(bufferOffset);
		recvData.DirectMoveDequeueOffset(sizeof(stMSG_HDR));
		bufferOffset += sizeof(stMSG_HDR);

		stMSG_GAME_REQ_ECHO* msg = (stMSG_GAME_REQ_ECHO*)recvData.GetBufferPtr(bufferOffset);
		bufferOffset += sizeof(stMSG_GAME_REQ_ECHO);

		temp_TotalRecvedMsg++;

		if (!Decode(hdr->randKey, hdr->len, hdr->checkSum, (BYTE*)msg)) {
			DebugBreak();
			// 연결 강제 종료?
		}

		if (msg->Type == en_PACKET_CS_GAME_REQ_ECHO) {
			ProcessEchoMessage(sessionID, msg->AccountoNo, msg->SendTick);
		}
		else {
			DebugBreak();
		}
	}

	//if (recvData.GetUseSize() != 0) {
	//	DebugBreak();
	//}
}
