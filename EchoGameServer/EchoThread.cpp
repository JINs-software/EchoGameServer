#include "EchoThread.h"
#include "CommonProtocol.h"

void EchoThread::ProcessEchoMessage(SessionID sessionID, INT64 accountNo, LONGLONG sendTick)
{
#if defined(ALLOC_BY_TLS_MEM_POOL)
	JBuffer* resMsg = AllocSerialSendBuff(sizeof(stMSG_GAME_RES_ECHO));
#else 
	JBuffer* resMsg = new JBuffer(sizeof(stMSG_HDR) + sizeof(stMSG_GAME_RES_ECHO));
	stMSG_HDR* hdr = resMsg->DirectReserve<stMSG_HDR>();
	hdr->code = dfPACKET_CODE;
	hdr->len = sizeof(stMSG_GAME_RES_ECHO);
	hdr->randKey = (BYTE)rand();
#endif
	
	*resMsg << (WORD)en_PACKET_CS_GAME_RES_ECHO;
	*resMsg << accountNo;
	*resMsg << sendTick;

	SendPacket(sessionID, resMsg);
}

void EchoThread::OnStart()
{
	//HANDLE hThread = GetCurrentThread();
	//
	//// 현재 스레드의 우선순위를 높음으로 설정
	//if (!SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST)) {
	//	std::cerr << "Failed to set thread priority. Error: " << GetLastError() << "\n";
	//	DebugBreak();
	//}
}

void EchoThread::OnMessage(SessionID sessionID, JBuffer& recvData)
{
	// 에코 요청 메시지 수신
	LONG recvMsgCnt = 0;
	while (recvData.GetUseSize() >= sizeof(WORD)) {
		WORD type;
		recvData.Peek(&type);
		if (type != en_PACKET_CS_GAME_REQ_ECHO) {
			DebugBreak();
		}
		else {
			stMSG_GAME_REQ_ECHO msg;
			recvData >> msg;
			ProcessEchoMessage(sessionID, msg.AccountoNo, msg.SendTick);
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
