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
