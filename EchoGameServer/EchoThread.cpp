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
	// => EchoGameServer������ SendPacket ���п� ���� �߰����� �۾� ����(?)
	SendPacket(sessionID, resMsg);
}

void EchoThread::OnRecv(SessionID sessionID, JBuffer& recvData)
{
	// ���� ��û �޽��� ����
	while (recvData.GetUseSize() >= sizeof(stMSG_HDR)) {
		stMSG_HDR msgHdr;
		recvData.Peek(&msgHdr);
		if (msgHdr.code != dfPACKET_CODE) {
			// �ڵ� ����ġ
			// ���� ���� ����!
			DebugBreak();
			break;
		}
		if (recvData.GetUseSize() < sizeof(stMSG_HDR) + msgHdr.len) {
			// �޽��� �̿ϼ�
			//DebugBreak();
			// => �޸� Ǯ ��Ŀ��� ����Ʈ ������ ��� ���� �� ��ȯ�� �޽��� ���� �߻�..
			break;
		}

		recvData >> msgHdr;
		if (!Decode(msgHdr.randKey, msgHdr.len, msgHdr.checkSum, recvData.GetDequeueBufferPtr())) {
			DebugBreak();
			// ���� ���� ����?
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
