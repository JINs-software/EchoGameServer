#include "EchoGameServer.h"
#include <conio.h>

int main() {
	EchoGameServer echoGameServer("127.0.0.1", 40000, 0, 4, 20000, SEND_BUFF_SIZE, RECV_BUFF_SIZE);

	echoGameServer.Start();

	char ctr;
	clock_t ct = 0;
	while (true) {
		if (_kbhit()) {
			ctr = _getch();
			if (ctr == 'q' || ctr == 'Q') {
				break;
			}
		}

		clock_t now = clock();
		if (now - ct > 1000) {
			echoGameServer.ConsoleLog();
			ct = now;
		}
	}

	echoGameServer.Stop();
}