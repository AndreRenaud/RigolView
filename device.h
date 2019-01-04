#ifndef RIGOL_DEVICE_H
#define RIGOL_DEVICE_H

int device_open(const char *device_name);
void device_close(int device);

int device_get_bmp(int dev, const char *bmp_file);

int device_get_keylock(int dev);
int device_set_keylock(int dev, int state);

#endif
