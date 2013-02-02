#ifndef INTERFACES_H
#define INTERFACES_H

typedef struct {
		u16int index;
} interface_info;

int read_interfaces(FILE* f, u16int interfaces_count, interface_info** interfaceinfo);
#endif
