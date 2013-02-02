#ifndef METHODS_H
#define METHODS_H
#include <attributes.h>
typedef struct
{
     u16int access_flags;
     u16int name_index;
     u16int descriptor_index;
     u16int attributes_count;
     attribute_info **attributes;
} method_info;

int read_methods(FILE* f, u16int methods_count, method_info **methodinfo);
#endif
