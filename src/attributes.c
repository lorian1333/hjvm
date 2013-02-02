#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>

#include <attributes.h>

int read_attributes(FILE* f, u16int attributes_count, attribute_info **attributeinfo)
{
     u8int* bytes = (u8int*) malloc(2);
     int i;
     attribute_info *info;
     size_t result;
     for(i=0;i<attributes_count;i++)
     {
          info = (attribute_info*) malloc(sizeof(attribute_info));
          READ_U2(bytes, f);
          info->attribute_name_index = MAKE_U16(bytes);
          READ_U4(bytes, f);
          info->attribute_length= MAKE_U32(bytes);
         
          info->info = (u8int*) malloc(info->attribute_length);
          result = fread((void*)info->info, 1, info->attribute_length, f);
          if(result != info->attribute_length) 
          {
               free(bytes);
               return 0;
          }
          //debugmsg("#%d name:%d length:%d\n", (i+1), info->attribute_name_index, info->attribute_length);
          
          attributeinfo[i] = info;
     }
     return 1;
}


Code_attribute* attribute_to_code(attribute_info *source)
{
     Code_attribute* code = (Code_attribute*) malloc(sizeof(Code_attribute));
     code->attribute_name_index = source->attribute_name_index;
     code->attribute_length = source->attribute_length;
     
     int i=0,j=0;
     

     code->max_stack = MAKE_U16((source->info + i));
     i+=2;
     code->max_locals = MAKE_U16((source->info + i));
     i+=2;
     code->code_length = MAKE_U32((source->info + i));
     i+=4;
     code->code = (u8int*) malloc(code->code_length);
     
     memcpy(code->code, source->info + i, code->code_length);
     i += code->code_length;
     code->exception_table_length = MAKE_U16((source->info + i));
     i+=2;
     exception_table *etable;
    
     for(j=0;j<code->exception_table_length;j++)
     {
          etable = (exception_table*) malloc(sizeof(exception_table));
          etable->start_pc = MAKE_U16((source->info + i));
          i+=2;
          etable->end_pc = MAKE_U16((source->info + i));
          i+=2;
          etable->handler_pc = MAKE_U16((source->info + i));
          i+=2;
          etable->catch_type = MAKE_U16((source->info + i));
          i+=2;
          code->exception_tables[j] = etable;
     }
     
     code->attributes_count = MAKE_U16((source->info + i));
     i+=2;
     code->attributes = (attribute_info**) malloc(code->attributes_count * sizeof(attribute_info*));
     attribute_info* att;
     
     
     for(j=0;j<code->attributes_count;j++)
     {
          att = malloc(sizeof(attribute_info));
          att->attribute_name_index = MAKE_U16((source->info + i));
          i+=2;
          att->attribute_length = MAKE_U32((source->info + i));

          i+=4;
          att->info = (u8int*) malloc(att->attribute_length);
          memcpy(att->info, source->info + i, att->attribute_length);
          i += att->attribute_length;
          code->attributes[j] = att;
     }
     
     return code;
}