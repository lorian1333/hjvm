#ifndef FIELD_H
#define FIELD_H

#include <attributes.h>

typedef struct {
     u16int access_flags;
     u16int name_index;
     u16int descriptor_index;
     u16int attributes_count;
     attribute_info** attributes;
    
} field_info;

int read_fields(FILE* f, u16int fields_count, field_info** fieldsinfo);

#endif
