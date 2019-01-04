#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#include "device.h"

static int device_command(int dev, const char *command, void *response, int max_resp_len)
{
	fprintf(stderr, "Command: %s\n", command);
	if (write(dev, command, strlen(command)) != strlen(command)) {
		return -1;
	}
	if (!response || max_resp_len <= 0)
		return 0;

	int len = read(dev, response, max_resp_len);
	if (len < 0)
		return -errno;

	return len;
}

int device_open(const char *device)
{
	int fd = open(device, O_RDWR);
	if (fd < 0)
		return -errno;
	return fd;
}


void device_close(int dev)
{
	close(dev);
}

int device_get_bmp(int dev, const char *file)
{
	int e;
	uint8_t data[81920];

	e = device_command(dev, ":DISP:DATA?", data, sizeof(data));
	if (e < 0)
		return e;

	//printf("Got %d bytes of wave data\n", e);

	FILE *fp = fopen(file, "wb");
	fwrite(&data[11], e - 11, 1, fp);
	fclose(fp);

	return 0;
}

int device_get_keylock(int dev)
{
	int e;
	char data[1024];

	e = device_command(dev, ":KEY:LOCK?", data, sizeof(data));
	if (e < 0)
		return e;
	data[e] = '\0';
	fprintf(stderr, "key:lock?: %s\n", data);
	return strcmp(data, "ENABLE") == 0;
}

int device_set_keylock(int dev, int state)
{
	int e;

	if (state)
		e = device_command(dev, ":KEY:LOCK ENAB", NULL, 0);
	else
		e = device_command(dev, ":KEY:LOCK DIS", NULL, 0);
	if (e < 0)
		return e;
	return device_get_keylock(dev) == state;
}
