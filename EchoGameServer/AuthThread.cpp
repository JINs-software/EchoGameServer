#include "AuthThread.h"
#include "Configuration.h"
#include "CommonProtocol.h"

void AuthThread::ProcessAuth(SessionID sessionID, INT64 accountNo, char sessionKey[64])
{
	// ���𽺿� ��û
	// ��û�� ���� ��ȯ�� ����
	if (AuthQueryToRedis(accountNo, sessionKey)) {
		// - ���� ����
		// (1) EnterGroup(����, ���� �׷�)
		ForwardSessionGroup(sessionID, ECHO_SESSION_GROUP);
		// (2) ���� ��ȯ �޽��� ����
		SendLoginResponse(sessionID, accountNo, 1);
	}
	else {
		// - ���� ����
		// (1) ���� ��ȯ �޽��� ����
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
	// ���� ����
}

void AuthThread::OnRecv(SessionID sessionID, JBuffer& recvData)
{
	// �α��� ��û �޽��� ����
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

		stMSG_GAME_REQ_LOGIN msg;
		recvData >> msg;
		if (msg.Type == en_PACKET_CS_GAME_REQ_LOGIN) {
			// 1. ���� Ȯ��
			//	skip..

			// 2. ����
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
