#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "ipc.h"

int
main(int argc, char * argv[]) {
	struct ipc ipc;
	struct msg msg = {
		.type = FADER_NOP,
		.value = 0,
	};

	ipc_connect(&ipc);
	ipc_msg_send(&ipc, &msg);
	ipc_close(&ipc);

	return 0;
}
