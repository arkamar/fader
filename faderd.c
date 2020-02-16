#include <fcntl.h>
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
main(int argc, char * argv[]) {
	int ret, max, bri, act;

	ret = chdir("/sys/class/backlight/intel_backlight");
	if (ret == -1) {
		perror("chdir");
		return 1;
	}

	act = get_value("actual_brightness");
	bri = get_value("brightness");
	max = get_value("max_brightness");

	fprintf(stderr, "act: %3d %3d%%\n", act, act * 100 / max);
	fprintf(stderr, "bri: %3d %3d%%\n", bri, bri * 100 / max);
	fprintf(stderr, "max: %3d %3d%%\n", max, max * 100 / max);

	return 0;
}
