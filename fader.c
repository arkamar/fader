#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "ipc.h"

void
usage(const char * argv0) {
		fputs("fader cmd\n", stderr);
		exit(1);
}

int
main(int argc, char * argv[]) {
	struct ipc ipc;
	enum fader_type type = FADER_SET;
	int value;

	char * argv0 = *argv; argv++; argc--;

	if (argc < 1) {
		usage(argv0);
	}

	const char * cmd = *argv; argv++; argc--;
	char * end;

	value = strtol(cmd, &end, 0);

	if (cmd[0] == '+' || cmd[0] == '-') {
		type = FADER_INC;
		if (*end == '%') {
			type = FADER_INC_PERC;
		}
	} else if (*end == '%') {
		type = FADER_SET_PERC;
	}

	struct msg msg = {
		.type = type,
		.value = value,
	};

	ipc_connect(&ipc);
	ipc_msg_send(&ipc, &msg);
	ipc_close(&ipc);

	return 0;
}
