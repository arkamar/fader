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

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

char * argv0;

static
struct ctx {
	int act_fd;
	int bri_fd;
	int max;
} ctx;

void
usage() {
	fputs("faderd\n", stderr);
	exit(0);
}

int
get_value(const int fd) {
	char buf[64];
	ssize_t ret;

	lseek(fd, 0L, SEEK_SET);
	ret = read(fd, buf, sizeof buf);
	if (ret <= 0) {
		perror("read");
		return -1;
	}

	buf[ret] = '\0';

	return atoi(buf);
}

int
set_value(const int fd, const int value) {
	char buf[64];
	int ret, len;

	len = sprintf(buf, "%d", value);
	lseek(fd, 0L, SEEK_SET);
	ret = write(fd, buf, len);
	if (ret != len) {
		perror("write");
		return -1;
	}
	return ret;
}

static
int
init_ctx(struct ctx * ctx) {
	int max_fd;

	memset(ctx, 0, sizeof * ctx);

	ctx->act_fd = open("actual_brightness", O_RDONLY);
	if (ctx->act_fd == -1) {
		perror("open actual_brightness");
		return -1;
	}

	max_fd = open("max_brightness", O_RDONLY);
	if (max_fd == -1) {
		perror("open max_brightness");
		return -1;
	}
	ctx->max = get_value(max_fd);
	close(max_fd);
	if (ctx->max == -1) {
		return -1;
	}

	ctx->bri_fd = open("brightness", O_RDWR);
	if (ctx->bri_fd == -1) {
		perror("open brightness");
		return -1;
	}

	return 0;
}

static
int
set_backlight(const enum fader_type type, int value) {
	int act, max = ctx.max;

	switch (type) {
	case FADER_SET:
		break;
	case FADER_SET_PERC:
		value = value * max / 100;
		break;
	case FADER_INC:
		act = get_value(ctx.act_fd);
		value = act + value;
		break;
	case FADER_INC_PERC:
		act = get_value(ctx.act_fd);
		value = act + value * max / 100;
		break;
	case FADER_DEC:
		act = get_value(ctx.act_fd);
		value = act - value;
		break;
	case FADER_DEC_PERC:
		act = get_value(ctx.act_fd);
		value = act - value * max / 100;
		break;
	case FADER_NOP:
		fputs("nop\n", stderr);
		return 0;
	default:
		fputs("Unknown type\n", stderr);
		return -1;
	};

	value = MIN(value, max);
	value = MAX(value, 1);
	set_value(ctx.bri_fd, value);

	return 0;
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

	ret = init_ctx(&ctx);
	if (ret == -1) {
		exit(1);
	}

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
			set_backlight(msg.type, msg.value);
		}

		act = get_value(ctx.act_fd);
		bri = get_value(ctx.bri_fd);
		max = ctx.max;

		fprintf(stderr, "act: %3d %3d%% bri: %3d %3d%% max: %3d %3d%%\n",
			act, act * 100 / max, bri, bri * 100 / max, max, max * 100 / max);
	}

	return 0;
}
