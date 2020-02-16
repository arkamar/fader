#include <stdio.h>
#include <unistd.h>

int
get_value(const char * name) {
	int value = -1;
	return value;
}

int
main(int argc, char * argv[]) {
	int ret;

	ret = chdir("/sys/class/backlight/intel_backlight");
	if (ret == -1) {
		perror("chdir");
		return 1;
	}

	return 0;
}
