#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char gpio_path[32];

	if (argc <= 1) {
		printf("please provide number of a sysfs exported GPIO (with edge configured)\n");
		exit(EXIT_FAILURE);
	}

	sprintf(gpio_path, "/sys/class/gpio/gpio%s/value", argv[1]);
	printf("waiting events/interrupts on %s\n", gpio_path);

	int fd = open(gpio_path, O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

        int nb;
	char value[2];

	/* Dummy read to start */
	lseek(fd, 0, SEEK_SET);
        read(fd, value, sizeof(*value));

	/* wait for "interrupt" */
	struct pollfd fds = {
		.fd = fd,
		.events = POLLPRI | POLLERR,
	};
	int ret = poll(&fds, 1, -1);

	/* consume it when it occurs */
	lseek(fd, 0, SEEK_SET);

	if (ret >= 1) {
		printf("event detected on %s, ", gpio_path);
		nb = read(fd, value, sizeof(*value));
		if (nb > 0) {
			printf("current state: %c", value[0]);
		}
		printf("\n");
	} else {
		perror("poll()");
		close(fd);
		exit(EXIT_FAILURE);
	}

	close(fd);
	exit(EXIT_SUCCESS);
}
