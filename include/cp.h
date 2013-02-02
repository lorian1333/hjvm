#ifndef CP_H
#define CP_H




enum ConstantType {
	CONSTANT_Class = 0x07,
	CONSTANT_Fieldref = 0x09,
	CONSTANT_Methodref = 0x0A,
	CONSTANT_InterfaceMethodref = 0x0B,
	CONSTANT_String = 0x08,
	CONSTANT_Integer = 0x03,
	CONSTANT_Float = 0x04,
	CONSTANT_Long = 0x05,
	CONSTANT_Double = 0x06,
	CONSTANT_NameAndType = 0x0C,
	CONSTANT_Utf8 = 0x01,
     CONSTANT_MethodHandle = 0x0F,
     CONSTANT_MethodType = 0x10,
     CONSTANT_InvokeDynamic = 0x12,
};

enum Method_types {
     REF_getField = 1,
     REF_getStatic = 2,
     REF_putField = 3,
     REF_putStatic = 4,
     REF_invokeVirtual = 5,	
     REF_invokeStatic = 6,
     REF_invokeSpecial = 7,
     REF_newInvokeSpecial = 8,	
     REF_invokeInterface = 9,
};

typedef struct {
		u8int tag;
		void* data;
} cp_info;

typedef struct {
    u8int name_index[2];
} CONSTANT_Class_info;

typedef struct {
    u8int class_index[2];
    u8int name_and_type_index[2];
}  CONSTANT_Fieldref_info;

typedef struct {
    u8int class_index[2];
    u8int name_and_type_index[2];
} CONSTANT_Methodref_info;

typedef struct {
    u8int class_index[2];
    u8int name_and_type_index[2];
}  CONSTANT_InterfaceMethodref_info ;

typedef struct {
    u8int string_index[2];
} CONSTANT_String_info;

typedef struct {
    u8int bytes[4];
	
} CONSTANT_Integer_info;
     
typedef struct {
    u8int bytes[4];
} CONSTANT_Float_info;

typedef struct {
    u8int high_bytes[4];
    u8int low_bytes[4];
} CONSTANT_Long_info;

typedef struct {
    u8int high_bytes[4];
    u8int low_bytes[4];
} CONSTANT_Double_info;


typedef struct {
    u8int name_index[2];
    u8int descriptor_index[2];
}  CONSTANT_NameAndType_info ;

typedef struct {
    u8int length[2];
    u8int* bytes;//[length];
} CONSTANT_Utf8_info;


typedef struct {
     u8int reference_kind;
     u8int reference_index[2];
} CONSTANT_MethodHandle_info;

typedef struct {
     u8int descriptor_index[2];
} CONSTANT_MethodType_info;

typedef struct {
     u8int bootstrap_method_attr_index[2];
     u8int name_and_type_index[2];
} CONSTANT_InvokeDynamic_info;


/*
typedef struct {
	u8int tag;
	u8int reference_kind;
	u8int reference_index[2];
} CONSTANT_MethodHandle_info ;
*/

u8int read_constant_pool(FILE* f, u16int *i, u16int *cpcount, cp_info** info);



#endif
