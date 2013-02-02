#ifndef JCF_H
#define JCF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>

#include <cp.h>
#include <interfaces.h>
#include <fields.h>
#include <methods.h>

/* Java class versions */
enum JavaVersion {
	J2SE7 = 0x33,
	J2SE6 = 0x32,
	J2SE5 = 0x31,
	JDK1_4 = 0x30,
	JDK1_3 = 0x2F,
	JDK1_2 = 0x2E,
	JDK1_1 = 0x2D,
};

/* Class's access flags */
enum ClassAccessFlags
{
     CLASS_ACC_PUBLIC =     0x0001,
     CLASS_ACC_FINAL  =     0x0010,
	CLASS_ACC_SUPER  =     0x0020,
     CLASS_ACC_INTERFACE =  0x0200,
	CLASS_ACC_ABSTRACT =   0x0400,
     CLASS_ACC_SYNTHETIC =  0x1000,
     CLASS_ACC_ANNOTATION = 0x2000,
     CLASS_ACC_ENUM =       0x4000,
};


/* Field's access flags */
enum FieldAccessFlags
{
     FIELD_ACC_PUBLIC =    0x001,
     FIELD_ACC_PRIVATE =   0x002,
     FIELD_ACC_PROTECTED = 0x004,
     FIELD_ACC_STATIC =    0x008,
	FIELD_ACC_FINAL  =    0x010,
     FIELD_ACC_VOLATILE =  0x040,
     FIELD_ACC_TRANSIENT = 0x080,
	FIELD_ACC_SYNTHETHC = 0x1000,
	FIELD_ACC_ENUM      = 0x4000,
};
/* Method's access flags */
enum MethodAccessFlags
{
     METHOD_ACC_PUBLIC =    0x001,
     METHOD_ACC_PRIVATE =   0x002,
     METHOD_ACC_PROTECTED = 0x004,
     METHOD_ACC_STATIC =    0x008,
	METHOD_ACC_FINAL  =    0x010,
     METHOD_ACC_SYNCHRONIZED = 0x020,
	METHOD_ACC_BRIDGE = 0x040,
	METHOD_ACC_VARARGS = 0x080,
     METHOD_ACC_NATIVE = 0x100,
     METHOD_ACC_ABSTRACT = 0x400,
     METHOD_ACC_STRICT = 0x800,
	METHOD_ACC_SYNTHETIC = 0x1000,
};

/* Java class file structure */
typedef struct 
{
	char* fn;
	u16int v_major, v_minor;
	u16int cpcount;
	cp_info **cp;
     u16int accflags;
	u16int this_class;
	u16int super_class;
	u16int interfaces_count;
     interface_info** interfaces;
     u16int fields_count;
     field_info **fields;
     u16int methods_count;
     method_info **methods;
     u16int attributes_count;
     attribute_info **attributes;
	
} JavaClassFile;

/* Primitive Java types */
typedef s8int J_byte;
typedef s16int J_short;
typedef s32int J_int;
typedef s64int J_long;
typedef u16int J_char;
typedef float32 J_float;
typedef float64 J_double;
typedef u8int J_boolean;

/* Other types */
// TODO: Check if this will work 
typedef void* J_returnAddress;
typedef void* J_reference;

typedef struct  {
J_int length;
u16int type;
J_reference* array; 
} J_reference_array;

/* Null */
#define J_null NULL

/* Error codes returned by Hjvm-functions */
enum HjvmErrorCodes
{
     E_METHOD_NOT_FOUND = 1,
     E_INVALID_METHOD_ARGUMENTS = 2,
     E_NULL_POINTER = 3,
     E_ABSTRACT = 4,
	E_OPCODE = 5,
     
	E_INTERNAL = 6,
	E_OUT_OF_MEMORY = 7,
	E_STACK_OVERFLOW = 8,
	E_UNKNOWN  = 9,
	E_EXCEPTION = 10,
};

/* Function declarations */ 
JavaClassFile* HjvmLoad(char* fn);
s8int HjvmExecute(JavaClassFile *jcf, char* methodname, u16int access_flags, u8int argc, void **argv, char* argformat, void *returnvalue);

/* Macros */
//#define STACK_PUSH(v,s,i) { s[++i] = v; }
//#define STACK_POP(m,s,i)  { m = s[i--]; }
#define STACK_PUSH(v) { stack[++stack_index] = v; }
#define STACK_POP(m)  { m = stack[stack_index--]; }

#define THROW_EXCEPTION(s) { err("Exception: java.lang.%s\n", s); return -E_EXCEPTION; }
#define CODE_READ(b) { b = code->code[++i]; }
#define CREATE_NEW_ARRAY(a,s) { a.length=s; a.type = 0; a.array = (J_reference*) malloc(s * sizeof(J_reference)); memset(a.array, 0, a.length); }
#define RETURN_FROM_METHOD(r) { returnvalue = r; free(stack); free(locals); return 0; }
#endif
