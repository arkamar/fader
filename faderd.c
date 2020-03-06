#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "arg.h"
#include "ipc.h"

char * argv0;

void
usage() {
	fputs("faderd\n", stderr);
	exit(0);
}

int
get_value(const char * name) {
	int fd;
	char buf[64];
	ssize_t ret;

	fd = open(name, O_RDONLY);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	ret = read(fd, buf, sizeof buf);
	close(fd);
	if (ret <= 0) {
		perror("read");
		return -1;
	}

	buf[ret] = '\0';

	return atoi(buf);
}

int
set_value(const char * name, const int value) {
	char buf[64];
	int ret, len;
	int fd;

	fd = open(name, O_WRONLY);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	len = sprintf(buf, "%d", value);
	ret = write(fd, buf, len);
	close(fd);
	if (ret != len) {
		perror("write");
		return -1;
	}
	return ret;
}

int
main(int argc, char * argv[]) {
	int ret, max, bri, act;
	char * backlight_device = "/sys/class/backlight/intel_backlight";
	struct pollfd pfd[2];
	struct ipc ipc;
	struct msg msg;

	ARGBEGIN {
	case 'd':
		backlight_device = EARGF(usage());
		break;
	case 'h':
	default:
		usage();
	} ARGEND

	ret = chdir(backlight_device);
	if (ret == -1) {
		perror("chdir");
		return 1;
	}

	ipc_bind(&ipc);

	pfd[0].fd = ipc.fd;
	pfd[0].events = POLLIN;

	while (1) {
		ret = poll(pfd, 1, -1);
		if (ret == -1) {
			perror("poll");
			break;
		}

		if (pfd[0].revents & POLLIN) {
			ipc_msg_recv(&ipc, &msg);
		}

		act = get_value("actual_brightness");
		bri = get_value("brightness");
		max = get_value("max_brightness");

		fprintf(stderr, "act: %3d %3d%% bri: %3d %3d%% max: %3d %3d%%\n",
			act, act * 100 / max, bri, bri * 100 / max, max, max * 100 / max);
	}

	return 0;
}
