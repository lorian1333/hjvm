#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>
#include <cp.h>
#include <util.h>




static inline void read_class(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Class_info *classinfo = (CONSTANT_Class_info*) malloc(sizeof(CONSTANT_Class_info));
	classinfo->name_index[0] = fgetc(f);
	classinfo->name_index[1] = fgetc(f);
	info->data = (void*) classinfo;
}

static inline void read_fieldref(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Fieldref_info *fieldrefinfo = (CONSTANT_Fieldref_info*) malloc(sizeof(CONSTANT_Fieldref_info));
	READ_U2(fieldrefinfo->class_index, f);
	READ_U2(fieldrefinfo->name_and_type_index, f);
	info->data = (void*) fieldrefinfo;
}

static inline void read_methodref(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Methodref_info *methodrefinfo = (CONSTANT_Methodref_info*) malloc(sizeof(CONSTANT_Methodref_info));
	READ_U2(methodrefinfo->class_index, f);
	READ_U2(methodrefinfo->name_and_type_index, f);
	info->data = (void*) methodrefinfo;
}

static inline void read_interfacemethodref(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_InterfaceMethodref_info *interfacemethodrefinfo = (CONSTANT_InterfaceMethodref_info*) malloc(sizeof(CONSTANT_InterfaceMethodref_info));
	READ_U2(interfacemethodrefinfo->class_index, f);
	READ_U2(interfacemethodrefinfo->name_and_type_index, f);
	info->data = (void*) interfacemethodrefinfo;
}

static inline void read_string(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_String_info* stringinfo = (CONSTANT_String_info*) malloc(sizeof(CONSTANT_String_info));
	READ_U2(stringinfo->string_index, f);
	info->data = (void*) stringinfo;
}

static inline void read_utf8(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Utf8_info* utf8info = (CONSTANT_Utf8_info*) malloc(sizeof(CONSTANT_Utf8_info));
	READ_U2(utf8info->length, f);
	utf8info->bytes = (u8int*) malloc(MAKE_U16(utf8info->length)+1);
     memset(utf8info->bytes, 0, MAKE_U16(utf8info->length)+1);
     //u8int byte;
	for(*i=0;*i<MAKE_U16(utf8info->length); (*i)++)
     {
          //TODO handle the modified UTF-8 encoding right 
          //http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.7
          
          utf8info->bytes[*i]  = fgetc(f);
          /*
		byte = fgetc(f);
          if(byte >= 0x0001 && byte <= 0x007f) utf8info->bytes[*i] = byte;
          
          else if(
          {
               debugmsg("sjkdasjdklasdjsdkadl  %X\n", ((byte & 0x1f) << 6) + (fgetc(f) & 0x3f));
               (*i)++;
          }
          */
     }
	
	info->data = (void*) utf8info;
}

static inline void read_integer(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Integer_info* intinfo = (CONSTANT_Integer_info*) malloc(sizeof(CONSTANT_Integer_info));
	READ_U4(intinfo->bytes, f);
	info->data = (void*) intinfo;
}

static inline void read_float(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Float_info* floatinfo = (CONSTANT_Float_info*) malloc(sizeof(CONSTANT_Float_info));
	READ_U4(floatinfo->bytes, f);
	info->data = (void*) floatinfo;
}

static inline void read_long(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Long_info* longinfo = (CONSTANT_Long_info*) malloc(sizeof(CONSTANT_Long_info));
	READ_U4(longinfo->high_bytes, f);
	READ_U4(longinfo->low_bytes, f);
	info->data = (void*) longinfo;
}

static inline void read_double(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_Double_info* doubleinfo = (CONSTANT_Double_info*) malloc(sizeof(CONSTANT_Double_info));
	READ_U4(doubleinfo->high_bytes, f);
	READ_U4(doubleinfo->low_bytes, f);
	info->data = (void*) doubleinfo;
}

static inline void read_nameandtype(FILE* f, u16int *i, cp_info *info)
{
	CONSTANT_NameAndType_info* nameandtypeinfo = (CONSTANT_NameAndType_info*) malloc(sizeof(CONSTANT_NameAndType_info));
	READ_U2(nameandtypeinfo->name_index, f);
	READ_U2(nameandtypeinfo->descriptor_index, f);
	info->data = (void*) nameandtypeinfo;
}

static inline void read_methodhandle(FILE* f, u16int *i, cp_info *info)
{
     CONSTANT_MethodHandle_info* methodhandleinfo = (CONSTANT_MethodHandle_info*) malloc(sizeof(CONSTANT_MethodHandle_info));
     methodhandleinfo->reference_kind = fgetc(f);
     READ_U2(methodhandleinfo->reference_index, f);
     info->data = (void*) methodhandleinfo;
}    

static inline void read_methodtype(FILE* f, u16int *i, cp_info *info)
{
     CONSTANT_MethodType_info* methodtypeinfo = (CONSTANT_MethodType_info*) malloc(sizeof(CONSTANT_MethodType_info));
     READ_U2(methodtypeinfo->descriptor_index, f);
     info->data = (void*) methodtypeinfo;
}

static inline void read_invokedynamic(FILE* f, u16int *i, cp_info *info)
{
     CONSTANT_InvokeDynamic_info* invokedynamicinfo = (CONSTANT_InvokeDynamic_info*) malloc(sizeof(CONSTANT_InvokeDynamic_info));
     READ_U2(invokedynamicinfo->bootstrap_method_attr_index, f);
     READ_U2(invokedynamicinfo->name_and_type_index, f);
     info->data = (void*) invokedynamicinfo;    
}
u8int read_constant_pool(FILE* f, u16int *i, u16int *cpcount, cp_info** cpinfo)
{

     u16int *j= (u16int*) malloc(sizeof(u16int));
	s32int* integer, *i2;
	u8int *raw = malloc(4), *raw2 = malloc(8);

     
     cp_info* info;

   
	debugmsg("Constant pool: \n");
	for(*i=0;*i < ((*cpcount)-1); (*i)++)
	{
        info = (cp_info*) malloc(sizeof(cp_info));
        memset(info, 0, sizeof(cp_info));
		info->tag = fgetc(f);
	
		switch(info->tag)
		{
			case CONSTANT_Class: 
			{
				read_class(f, j, info);
				debugmsg("#%d Class #%d\n", (*i)+1, MAKE_U16( ((CONSTANT_Class_info*) info->data)->name_index ));
				break;
			}
			
			case CONSTANT_Fieldref:
			{
				read_fieldref(f, j, info);
				 debugmsg("#%d Fieldref #%d.#%d\n", (*i)+1,
				MAKE_U16( ((CONSTANT_Fieldref_info*)info->data)->class_index  ),
				MAKE_U16( ((CONSTANT_Fieldref_info*)info->data)->name_and_type_index ));
				break;
			}
			case CONSTANT_Methodref:
			{
				read_methodref(f, j, info);
				 debugmsg("#%d Methodref #%d.#%d\n", (*i)+1,
				MAKE_U16( ((CONSTANT_Methodref_info*)info->data)->class_index ),
				MAKE_U16( ((CONSTANT_Methodref_info*)info->data)->name_and_type_index ));
				break;
			}
			case CONSTANT_InterfaceMethodref:
			{
				read_interfacemethodref(f, j, info);
				 debugmsg("#%d InterfaceMethodref #%d.#%d\n", (*i)+1,
				MAKE_U16( ((CONSTANT_InterfaceMethodref_info*)info->data)->class_index  ),
				MAKE_U16( ((CONSTANT_InterfaceMethodref_info*)info->data)->name_and_type_index  ));
				break;
			}
			case CONSTANT_String:
			{
				read_string(f, j, info);
				debugmsg("#%d String #%d\n", (*i)+1, MAKE_U16( ((CONSTANT_String_info*)info->data)->string_index));
				break;
			}
			case CONSTANT_Integer:
			{
				read_integer(f, j, info);
				
				MAKE_S32(raw, ((CONSTANT_Integer_info*)info->data)->bytes);
				integer = (s32int*) raw;
				debugmsg("#%d Integer %d\n", (*i) + 1, *integer);
				break;
			}
			case CONSTANT_Float:
			{	
				read_float(f, j, info);
				COPY_FLOAT(raw, ((CONSTANT_Float_info*)info->data)->bytes);
				debugmsg("#%d Float %f\n", (*i)+1, *((float32*) raw));
				break;
			}
			case CONSTANT_Long:
			{
				read_long(f, j, info);
				MAKE_S64(raw2, ((CONSTANT_Long_info*)info->data)->high_bytes );
				debugmsg("#%d Long %ld\n", (*i)+1,  *( (s64int*) raw2));
                    cpinfo[(*i)++] = NULL;
				break;
			}
			case CONSTANT_Double:
			{
				read_double(f, j, info);
				COPY_DOUBLE(raw2, ((CONSTANT_Double_info*) info->data)->high_bytes);
				debugmsg("#%d Double %f\n", (*i)+1, *( (float64*) raw2));
				cpinfo[(*i)++] = NULL;
				break;
			}
			case CONSTANT_NameAndType:
			{
				read_nameandtype(f, j, info);
				debugmsg("#%d NameAndType #%d.#%d\n", (*i)+1, MAKE_U16( ((CONSTANT_NameAndType_info*) info->data)->name_index) , MAKE_U16(((CONSTANT_NameAndType_info*) info->data)->descriptor_index ) );
				break;
			}
			case CONSTANT_Utf8:
			{
				read_utf8(f, j, info);
                    debugmsg("#%d Utf8 %s\n", (*i)+1, utf8_to_cstring(((CONSTANT_Utf8_info*) info->data)->bytes));
				break;
			}
               case CONSTANT_MethodHandle:
               {
                    read_methodhandle(f, j, info);
                    debugmsg("#%d MethodHandle Kind:%d #%d\n", *(i)+1, ((CONSTANT_MethodHandle_info*) info->data)->reference_kind, MAKE_U16(((CONSTANT_MethodHandle_info*) info->data)->reference_index));
                    break;
               }
               
               case CONSTANT_MethodType:
               {
                    read_methodtype(f, j, info);
                    debugmsg("#%d MethodType #%d\n", *(i)+1, MAKE_U16(((CONSTANT_MethodType_info*) info->data)->descriptor_index));
                    break;
               }
               
               case CONSTANT_InvokeDynamic:
               {
                    read_invokedynamic(f,j,info);
                    debugmsg("#%d InvokeDynamic #%d.#%d\n", *(i)+1, MAKE_U16(((CONSTANT_InvokeDynamic_info*) info->data)->bootstrap_method_attr_index),
                              MAKE_U16(((CONSTANT_InvokeDynamic_info*) info->data)->name_and_type_index));
                    break;
               }
               
			default: {
				debugerr("Unrecognized tag: %02X!\n", info->tag);
				free(info);
				return 0;
					}
		}
          cpinfo[(*i)] = info;       
	}
     
	 debugmsg("Done reading Constant Pool\n");
      

	free(j);
	free(raw);
	free(raw2);
     
     
	return 1;
}

