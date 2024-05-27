#include "EchoGameServer.h"
#include "CommonProtocol.h"

void EchoGameServer::OnClientJoin(SessionID sessionID)
{
	EnterSessionGroup(sessionID, AUTH_SESSION_GROUP);
}

void EchoGameServer::OnClientLeave(SessionID sessionID)
{
	LeaveSessionGroup(sessionID);
}

UINT EchoGameServer::RecvData(JBuffer& recvBuff, JBuffer& dest)
{
	UINT retRecvedPacketCnt = 0;

	while (recvBuff.GetUseSize() >= sizeof(stMSG_HDR)) {
		stMSG_HDR hdr;
		recvBuff.Peek(&hdr);
		if (recvBuff.GetUseSize() < sizeof(stMSG_HDR) + hdr.len) {
			break;
		}
		if (dest.GetFreeSize() < sizeof(stMSG_HDR) + hdr.len) {
			break;
		}

		retRecvedPacketCnt++;

		if (recvBuff.GetDirectDequeueSize() >= sizeof(stMSG_HDR) + hdr.len) {
			dest.Enqueue(recvBuff.GetDequeueBufferPtr(), sizeof(stMSG_HDR) + hdr.len);
		}
		else {
			UINT dirDeqSize = recvBuff.GetDirectDequeueSize();
			dest.Enqueue(recvBuff.GetDequeueBufferPtr(), dirDeqSize);
			dest.Enqueue(recvBuff.GetBeginBufferPtr(), sizeof(stMSG_HDR) + hdr.len - dirDeqSize);
		}
		recvBuff.DirectMoveDequeueOffset(sizeof(stMSG_HDR) + hdr.len);
	}

	return retRecvedPacketCnt;
}
