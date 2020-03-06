#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
	struct pollfd pfd[2];

	ret = chdir("/sys/class/backlight/intel_backlight");
	if (ret == -1) {
		perror("chdir");
		return 1;
	}

	while (1) {
		ret = poll(pfd, 0, 10000);
		if (ret == -1) {
			perror("poll");
			break;
		}

		act = get_value("actual_brightness");
		bri = get_value("brightness");
		max = get_value("max_brightness");

		fprintf(stderr, "act: %3d %3d%% bri: %3d %3d%% max: %3d %3d%%\n",
			act, act * 100 / max, bri, bri * 100 / max, max, max * 100 / max);
	}

	return 0;
}
