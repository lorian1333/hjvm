#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H
typedef struct 
{
     u16int attribute_name_index;
     u32int attribute_length;
     u8int *info;
} attribute_info;

typedef struct 
{
     u16int attribute_name_index;
     u32int attribute_length;
     u16int constantvalue_index;
} ConstantValue_attribute;

typedef struct
{
     u16int start_pc;
     u16int end_pc;
     u16int handler_pc;
     u16int catch_type;
} exception_table;

typedef struct
{
     u16int attribute_name_index;
     u32int attribute_length;
     u16int max_stack;
     u16int max_locals;
     u32int code_length;
     u8int* code;
     u16int exception_table_length;
     exception_table **exception_tables;
     u16int attributes_count;
     attribute_info **attributes;
} Code_attribute;

/* FIXME This should not be empty */
typedef struct {} stack_map_frame;
typedef struct {
	u16int attribute_name_index;
	u32int attribute_length;
	u16int number_of_entries;
	stack_map_frame** entries;
} StackMapTable_attribute;

typedef struct {
     u16int attribute_name_index;
     u32int attribute_length;
     u16int number_of_exceptions;
     u16int *exception_index_table;
} Exceptions_attribute;

typedef struct 
{
     u16int inner_class_info_index;
     u16int outer_class_info_index;
     u16int inner_name_index;
     u16int inner_class_access_flags;
} class_struct;

typedef struct
{   
     u16int attribute_name_index;
     u32int attribute_length;
     u16int number_of_classes;
     class_struct **classes;
} InnerClasses_attribute;

typedef struct 
{
	u16int attribute_name_index;
     u32int attribute_length;
	u16int class_index;
	u16int method_index;
} EnclosingMethod_attribute;

typedef struct
{
     u16int attribute_name_index;
     u32int attribute_length;
} Synthetic_attribute;

typedef struct {
	u16int attribute_name_index;
     u32int attribute_length;
	u16int signature_index;
} Signature_attribute;

typedef struct
{
     u16int attribute_name_index;
     u32int attribute_length;
     u16int sourcefile_index;
} SourceFile_attribute;

typedef struct
{
	u16int attribute_name_index;
     u32int attribute_length;
	u8int* debug_extensions;
} SourceDebugExtension_attribute;

typedef struct
{
     u16int start_pc;
     u16int line_number;
} line_number_table_struct;

typedef struct
{
     u16int attribute_name_index;
     u32int attribute_length;
     u16int line_number_table_length;
     line_number_table_struct **line_number_table;
} LineNumberTable_attribute;

typedef struct
{
     u16int start_pc;
     u16int length;
     u16int name_index;
     u16int descriptor_index;
     u16int index;
} local_variable_table_struct;

typedef struct 
{
     u16int attribute_name_index;
     u32int attribute_length;
     u16int local_variable_table_lenght;
     local_variable_table_struct **local_variable_table;
} LocalVariableTable_attribute;

typedef struct
{
	u16int start_pc;
     u16int length;
     u16int name_index;
     u16int signature_index;
     u16int index;
} local_variable_type_table_struct;

typedef struct
{
	u16int attribute_name_index;
     u32int attribute_length;
	u16int local_variable_type_table_length;
	local_variable_type_table_struct** local_variable_type_table;
} LocalVariableTypeTable_attribute;

typedef struct
{
     u16int attribute_name_index;
     u32int attribute_length;
} Deprecated_attribute;

// TODO: add the rest of the attribute structures 

int read_attributes(FILE* f, u16int attributes_count, attribute_info **attributeinfo);
Code_attribute* attribute_to_code(attribute_info *source);

#endif
