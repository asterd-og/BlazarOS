#include <dev/dev.h>

dev_info* dev_list[256];
int dev_idx = 0;

void dev_register(const char* name, u8 type, int (*read)(u8*,u32,u32), int (*write)(u8*,u32,u32)) {
    dev_info* device = (dev_info*)kmalloc(sizeof(dev_info));
    memcpy(device->name, name, strlen(name));
    device->type = type;
    device->read = read;
    device->write = write;
    dev_list[dev_idx] = device;
    dev_idx++;
}

dev_info* dev_get(const char* name) {
    for (int i = 0; i < dev_idx; i++) {
        if (!strcmp(dev_list[i]->name, name)) {
            return dev_list[i];
        }
    }
    return NULL;
}