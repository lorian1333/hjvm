#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <fields.h>



int read_fields(FILE* f, u16int fields_count, field_info** fieldsinfo)
{
     
     u8int* bytes = (u8int*) malloc(2);
     field_info* info;
     int i,j;
     
     for(i=0;i<fields_count;i++)
     {
          info = (field_info*) malloc(sizeof(field_info));
          READ_U2(bytes, f);
          info->access_flags = MAKE_U16(bytes);
          READ_U2(bytes, f);
          info->name_index = MAKE_U16(bytes);
          READ_U2(bytes, f);
          info->descriptor_index = MAKE_U16(bytes);
          READ_U2(bytes, f);
          info->attributes_count = MAKE_U16(bytes);
          info->attributes = (attribute_info**) malloc(info->attributes_count * sizeof(attribute_info*));
          if(!read_attributes(f, info->attributes_count, info->attributes))
          {
               free(bytes);
               return 0;
          }
          
          //debugmsg("#%d n:%d d:%d a:%d\n", (i+1), info->name_index, info->descriptor_index, info->attributes_count);
          fieldsinfo[i] = info;
     }
     return 1;
}
