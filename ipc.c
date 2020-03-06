#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include "ipc.h"

static const
struct sockaddr_un addr = {
	.sun_family = AF_UNIX,
	.sun_path = "/tmp/fader.sock"
};

static
int
init_socket(struct ipc * ipc) {
	ipc->fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (ipc->fd == -1) {
		perror("socket");
		return -1;
	}
	return 0;
}

int
ipc_bind(struct ipc * ipc) {
	int ret = init_socket(ipc);
	if (ret == -1) {
		return ret;
	}

	unlink(addr.sun_path);

	if (bind(ipc->fd, (struct sockaddr *)&addr, sizeof addr) == -1) {
		perror("connect");
		return -1;
	}

	chmod(addr.sun_path, 0777);

	return 0;
}

int
ipc_connect(struct ipc * ipc) {
	int ret = init_socket(ipc);
	if (ret == -1) {
		return ret;
	}

	if (connect(ipc->fd, (struct sockaddr *)&addr, sizeof addr) == -1) {
		perror("connect");
		return -1;
	}

	return 0;
}

int
ipc_close(struct ipc * ipc) {
	return close(ipc->fd);
}

int
ipc_msg_send(struct ipc * ipc, const struct msg * msg) {
	ssize_t ret;

	ret = write(ipc->fd, msg, sizeof * msg);

	if (ret != sizeof * msg)
		return -1;

	return 0;
}

int
ipc_msg_recv(struct ipc * ipc, struct msg * msg) {
	ssize_t ret;

	ret = read(ipc->fd, msg, sizeof * msg);

	if (ret != sizeof * msg)
		return -1;

	return 0;
}
