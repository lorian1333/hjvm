#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>

#include <methods.h>

int read_methods(FILE* f, u16int methods_count, method_info **methodinfo)
{
     u8int* bytes = (u8int*) malloc(2);
     method_info *info;
     int i;
     for(i=0;i<methods_count;i++)
     {
          info = (method_info*) malloc(sizeof(method_info));
          READ_U2(bytes,f);
          info->access_flags = MAKE_U16(bytes);
          READ_U2(bytes,f);
          info->name_index = MAKE_U16(bytes);
          READ_U2(bytes,f);
          info->descriptor_index = MAKE_U16(bytes);
          READ_U2(bytes,f);
          info->attributes_count = MAKE_U16(bytes);
          
          info->attributes = (attribute_info**) malloc(info->attributes_count * sizeof(attribute_info*));
          if(!read_attributes(f, info->attributes_count, info->attributes))
          {
               free(bytes);
               return 0;
          }
          //debugmsg("#%d name:#%d descriptor:#%d\n", (i+1), info->name_index, info->descriptor_index);
          methodinfo[i] = info;
          
     }
    
     
     free(bytes);
     return 1;
}

