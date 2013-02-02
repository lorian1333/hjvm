#include <jcf.h>
#include <opcodes.h>

static inline u8int check_magic(FILE* f)
{
	u8int *magic= (u8int*) malloc(4);
	READ_U4(magic, f);
	debugmsg("MAGIC = 0x%02X%02X%02X%02X\n", magic[0], magic[1], magic[2], magic[3]);
	if(magic[0]==0xCA && magic[1]==0xFE && magic[2]==0xBA && magic[3]==0xBE) {
		free(magic);
          debugmsg("MAGIC OK!\n");
		return 1;
	}
	else 
	{
		free(magic);
          debugmsg("WRONG MAGIC! Not a class file.\n");
		return 0;
	}
	
}

static inline void read_version(FILE* f, u16int* major, u16int* minor)
{

	u8int *minor_bytes = (u8int*) malloc(2), *major_bytes = (u8int*) malloc(2);
	READ_U2(minor_bytes, f);
	READ_U2(major_bytes, f);
	if(minor!=NULL) *minor = (minor_bytes[0]*0x100) | minor_bytes[1];
	if(major!=NULL) *major = (major_bytes[0]*0x100) | major_bytes[1];
	free(minor_bytes);
	free(major_bytes);
}

static inline void read_cp_count(FILE* f, u16int *cpcount)
{
	u8int *cpcount_bytes = (u8int*) malloc(2);
	READ_U2(cpcount_bytes, f);
	if(cpcount != NULL) *cpcount = MAKE_U16(cpcount_bytes);
	free(cpcount_bytes);
}

static inline void read_access_flags(FILE* f, u16int* accflags)
{
	u8int* acc_flag_bytes = (u8int*) malloc(2);
	READ_U2(acc_flag_bytes, f);
	if(accflags != NULL) *accflags = MAKE_U16(acc_flag_bytes);
	free(acc_flag_bytes);
}
static inline void read_this_and_super_class(FILE* f, u16int* this_class, u16int* super_class)
{
     u8int* bytes = (u8int*) malloc(2);
     READ_U2(bytes, f);
     if(this_class != NULL) *this_class =  MAKE_U16(bytes); 
     READ_U2(bytes, f);
     if(super_class != NULL) *super_class = MAKE_U16(bytes); 
     free(bytes);
}

static inline void read_interfaces_count(FILE* f, u16int *interfacescount)
{
	u8int *interfacescount_bytes = (u8int*) malloc(2);
	READ_U2(interfacescount_bytes, f);
	if(interfacescount != NULL) *interfacescount = MAKE_U16(interfacescount_bytes);
	free(interfacescount_bytes);
}

static inline void read_fields_count(FILE* f, u16int *fields_count)
{
     u8int* fieldscount_bytes = (u8int*) malloc(2);
     READ_U2(fieldscount_bytes, f);
     if(fields_count != NULL) *fields_count = MAKE_U16(fieldscount_bytes);
     free(fieldscount_bytes);
}

static inline void read_methods_count(FILE* f, u16int *methods_count)
{
     u8int* methodscount_bytes = (u8int*) malloc(2);
     READ_U2(methodscount_bytes, f);
     if(methods_count != NULL) *methods_count = MAKE_U16(methodscount_bytes);
     free(methodscount_bytes);
}

static inline void read_attributes_count(FILE* f, u16int *attributes_count)
{
     u8int* attributes_count_bytes = (u8int*) malloc(2);
     READ_U2(attributes_count_bytes, f);
     if(attributes_count != NULL) *attributes_count = MAKE_U16(attributes_count_bytes);
     free(attributes_count_bytes);
}
JavaClassFile* HjvmLoad(char* fn)
{
     /* Opening file */
	JavaClassFile* jcf = (JavaClassFile*) malloc(sizeof(JavaClassFile));
	jcf->fn = fn;
	verbosemsg("Loading classfile %s\n", fn);
	u16int i;
	FILE* f = fopen(fn, "rb");
	if(f==NULL)
	{
		errwithmsg("Unable to open file");
		return NULL;
	}
     /* Check if magic==ok */
	if(!check_magic(f))
	{
		debugerr("Invalid magic!\n");
		return NULL;
	}
     
     /* Read class version */
	u16int minor, major;
	read_version(f, &major, &minor);
	verbosemsg("Class version: %d.%d\n", major, minor);
	
	if(major < MIN_CLASS_VERSION) 
	{
		debugerr("Version too low!\n");
		return NULL;
	}
	else if(major > J2SE7 )
	{
		debugmsg("WARNING: Version > 51.0 (J2SE7)");
	}
	jcf->v_major = major;
     jcf->v_minor = minor;
	
	
	/* Read cpcount and constant_pool */
     
	u16int cpcount;
	read_cp_count(f, &cpcount);
	verbosemsg("Constant pool count: %d\n", cpcount);
     jcf->cpcount = cpcount;
     
	cp_info** cpinfo = (cp_info**) malloc( (cpcount-1) * sizeof(cp_info*));
	memset(cpinfo, 0, sizeof(cp_info*) * ( cpcount-1));

	if(!read_constant_pool(f, &i, &cpcount, cpinfo)) return NULL;
     jcf->cp = cpinfo;
   
     /* Read accessflags */
	u16int accflags;
	read_access_flags(f, &accflags);
     jcf->accflags = accflags;
     
	if(hjvm_be_extra_verbose())
	{     
	    char *public_str = "", *final_str = "", *super_str = "", *interface_str = "", *abstract_str = "";
		if(accflags & CLASS_ACC_PUBLIC) public_str = "ACC_PUBLIC, ";
		if(accflags & CLASS_ACC_FINAL) final_str = "ACC_FINAL, ";
		if(accflags & CLASS_ACC_SUPER) super_str = "ACC_SUPER, ";
		if(accflags & CLASS_ACC_INTERFACE) interface_str = "ACC_INTERFACE, ";
		if(accflags & CLASS_ACC_ABSTRACT) abstract_str = "ACC_ABSTRACT, ";
        debugmsg("Access flags: %s%s%s%s%s\n", public_str, final_str, super_str, interface_str, abstract_str);
	}
	if(accflags & CLASS_ACC_INTERFACE) 
	{
          debugmsg("This is classfile defines an interface\n");
		if(!(accflags & CLASS_ACC_ABSTRACT))
		{
			debugerr("This classfile is an interface, but ACC_ABSTRACT was not set in access flags\n");
		}
		if(accflags & CLASS_ACC_FINAL)
		{
			debugerr("This classfile is an interface, but ACC_FINAL was also set in access flags\n");
		}
		if(accflags & CLASS_ACC_SUPER) 
		{
			debugerr("This classfile is an interface, but ACC_SUPER was also set in access flags\n");
		}
          if(accflags & CLASS_ACC_ENUM)
          {
               debugerr("This classfile is an interface, but ACC_ENUM was also set in access flags\n");
          }
		if((accflags & CLASS_ACC_FINAL) || (accflags & CLASS_ACC_SUPER) || (!(accflags & CLASS_ACC_ABSTRACT))) 
          {
               err("Incorrect file.\n");
               return NULL;
          }
		
	}
	else
	{
		debugmsg("This classfile defines a class\n"); 
		
          
		if( (accflags & CLASS_ACC_FINAL) && (accflags & CLASS_ACC_ABSTRACT))
		{
			debugerr("This classfile is a class, but ACC_FINAL and ACC_ABSTRACT were both set in access flags\n");
               err("Incorrect file.\n");
			return NULL; 
		}
          
		
	} 
	
     /* read this_class and super_class */
     
     u16int this_class, super_class;
     read_this_and_super_class(f, &this_class, &super_class);
     u16int utf8index;
     if(hjvm_be_verbose())
     {
          debugmsg("this_class = %d\n", this_class);
          utf8index = MAKE_U16(((CONSTANT_Class_info*)jcf->cp[this_class-1]->data)->name_index)-1;
          verbosemsg("This class: %s\n", ((CONSTANT_Utf8_info*)jcf->cp[utf8index]->data)->bytes);
          if(super_class != 0)
          {
               debugmsg("super_class = %d\n", super_class);
               utf8index = MAKE_U16(((CONSTANT_Class_info*)jcf->cp[super_class-1]->data)->name_index)-1;
               verbosemsg("Superclass: %s\n", ((CONSTANT_Utf8_info*)jcf->cp[utf8index]->data)->bytes);
          }
          else
          {
               verbosemsg("This class does not have a superclass\n");
               
          }
     }
    
     if(super_class=0)
     {
          utf8index = MAKE_U16(((CONSTANT_Class_info*)jcf->cp[this_class-1]->data)->name_index)-1;
          if(strcmp(((CONSTANT_Utf8_info*)jcf->cp[utf8index]->data)->bytes , "java/lang/Object")!=0)
          {
               debugerr("This class does not have a superclass, but does not define the java.lang.Object class\n");
               return NULL;
          }
     }
     jcf->this_class = this_class;
     jcf->super_class = super_class;
     
     /* read interfaces_count and interfaces */
     
     u16int interfaces_count;
     read_interfaces_count(f, &interfaces_count);
     jcf->interfaces_count = interfaces_count;
     verbosemsg("Interfaces count: %d\n", interfaces_count);
     
     u16int *cp_index = (u16int*) malloc(sizeof(u16int));
     if(interfaces_count > 0)
     {
          interface_info** interfaceinfo = (interface_info**) malloc(interfaces_count * sizeof(interface_info*));
          if(!read_interfaces(f, interfaces_count, interfaceinfo)) return NULL;
          
          jcf->interfaces = interfaceinfo;
         
          char* message = (char*) malloc(1000);
          memset(message, 0, 1000); 
          strcpy(message, "Interfaces: ");

          char* interfacename = (char*) malloc(100);
          memset(interfacename, 0, 100);
          
          if(hjvm_be_verbose())
          {
               for(i=0;i<interfaces_count;i++)
               {
                    
                    *cp_index = MAKE_U16(((CONSTANT_Class_info*) jcf->cp[interfaceinfo[i]->index-1]->data)->name_index);
                    strcpy(interfacename, ((CONSTANT_Utf8_info*) jcf->cp[(*cp_index)-1]->data)->bytes);
                   

                    if(i==0) sprintf(message, "%s%s", message, interfacename);
                    else sprintf(message, "%s, %s", message, interfacename);
                    
                   
               }
          }
          
          sprintf(message, "%s\n", message);
          verbosemsg(message);
          free(interfacename);
          free(message);
          
     }
     
    /* Read fields_count and fields */
    
    u16int fields_count;
    read_fields_count(f, &fields_count);
    verbosemsg("Fields count: %d\n", fields_count);
    jcf->fields_count = fields_count;
    field_info** fieldinfo = (field_info**) malloc(fields_count * sizeof(field_info*));
    
    if(!read_fields(f, fields_count, fieldinfo)) return NULL;
    
    jcf->fields = fieldinfo;
    
    /* Read methods_count and methods */
    
    u16int methodscount;
    read_methods_count(f, &methodscount);
    verbosemsg("Methods count: %d\n", methodscount);
    jcf->methods_count = methodscount;
    method_info** methodinfo = (method_info**) malloc(methodscount * sizeof(method_info*));
    
    if(!read_methods(f, methodscount, methodinfo)) return NULL;   
    jcf->methods = methodinfo;
    
    
    /* Read attributes_count and attributes */
    
    u16int attributescount;
    read_attributes_count(f, &attributescount);
    verbosemsg("Attributes count: %d\n", attributescount);
    jcf->attributes_count = attributescount;
    
    jcf->attributes = (attribute_info**) malloc(attributescount * sizeof(attribute_info*));
    if(!read_attributes(f, attributescount, jcf->attributes)) return NULL;
    
    fclose(f);
    return jcf;
}

static inline char* substring(char* source, u8int beginindex, u8int endindex)
{
     char* ret = (char*) malloc(endindex-beginindex);
     u8int i;
     for(i=beginindex;i<endindex;i++)
          ret[i-beginindex] = source[i];
          
     return ret;
}



static JavaClassFile *g_jcf;

static inline u8int check_if_compatible(u16int type1, u16int type2)
{
	//TODO: Make this work
	return 1;
}
s8int HjvmExecuteCode(Code_attribute* code, Exceptions_attribute* exceptions, u8int argc, void **argv, void *returnvalue)
{
	/* Allocate and initialze the stack */
	void** stack = (void**) malloc((code->max_stack+1) * sizeof(void*));
	memset(stack, 0, (code->max_stack+1) * sizeof(void*));
	u16int stack_index=0;
	STACK_PUSH(J_null);
	stack_index = 0;
	
	debugmsg("max stack: %d\n", code->max_stack);
	
	/* Allocate and initialize the locals */
	void **locals = (void**) malloc((code->max_locals) * sizeof(void*));
	memset(locals, 0, (code->max_locals+1) * sizeof(void*));
	//u16int locals_index=0;
	//STACK_PUSH(J_null, locals, locals_index);
	//locals_index=0;
	debugmsg("max locals: %d\n", code->max_locals);
	
	
	/* Read and execute the code */
	u32int i;
	debugmsg("code length: %d\n", code->code_length);
		
	u8int opcode;     // Holds the current opcode
	void *m1,*m2,*m3; // Hold pop-ed stack information
	u8int b1 = 0, b2 = 0, b3 = 0;   // Hold operands
	
	for(i=0;i<code->code_length;i++)
	{
		/* Read the opcode */ 
		opcode = code->code[i];
		/* Execute it */
		switch(opcode)
		{
			case OPCODE_aaload: //throws NullPointerException, ArrayIndexOutOfBoundsException
			{	
				STACK_POP(m2); //index
				STACK_POP(m1); //arrayref
				if( ((J_reference_array*) m1)== J_null) THROW_EXCEPTION("NullPointerException");
				if( (*((J_int*) m2)) >= ((J_reference_array*)m1)->length) THROW_EXCEPTION("ArrayIndexOutOfBoundsException");
				STACK_PUSH( &(((J_reference_array*) m1)->array[*((J_int*) m2)]->reference) ); //value
				break;
			}
			case OPCODE_aastore: //throws NullPointerException, ArrayIndexOutOfBoundsException, ArrayStoreException
			{ 
				STACK_POP(m3); //value
				STACK_POP(m2); //index
				STACK_POP(m1); //arrayref
				if(((J_reference_array*) m1) == J_null) THROW_EXCEPTION("NullPointerException");
				if( (*((J_int*) m2)) >= ((J_reference_array*)m1)->length) THROW_EXCEPTION("ArrayIndexOutOfBoundsException");

				if( !check_if_compatible(((J_reference_array*) m1)->type, ((J_reference*) m3)->component_type )) THROW_EXCEPTION("ArrayStoreException");
				((J_reference_array*) m1)->array[*((J_int*) m2)]->reference = m3;
				break;
			} 
			case OPCODE_aconst_null:
			{ 
				STACK_PUSH(J_null);
				break;
			} 
			case OPCODE_aload:
			{ 
				CODE_READ(b1); //index
				STACK_PUSH(locals[b1]); //objectref
				break;
			} 
			case OPCODE_aload_0:
			{ 
				STACK_PUSH(locals[0]); //objectref
				break;
			} 
			case OPCODE_aload_1:
			{ 
				STACK_PUSH(locals[1]); //objectref
				break;
			} 
			case OPCODE_aload_2:
			{ 
				STACK_PUSH(locals[2]); //objectref
				break;
			} 
			case OPCODE_aload_3:
			{ 
				STACK_PUSH(locals[3]); //objectref
				break;
			} 
			case OPCODE_anewarray: //throws NegativeArraySizeException
			{ 

				CODE_READ(b1); //indexbyte1
				CODE_READ(b2); //indexbyte2
				STACK_POP(m1); //count
				debugmsg("count: %d\n", (*((J_int*) m1)) );
				if( (*((J_int*) m1)) < 0) THROW_EXCEPTION("NegativeArraySizeException");
				
				J_reference_array array;
				CREATE_NEW_ARRAY(array, (*((J_int*) m1)));

				((CONSTANT_Utf8_info*) g_jcf->cp[MAKE_U16(((CONSTANT_Class_info*) g_jcf->cp[((b1 << 8) | b2)]->data)->name_index)]->data);
				array.type = MAKE_U16(((CONSTANT_Class_info*) g_jcf->cp[((b1 << 8) | b2)]->data)->name_index);
				//debugmsg("i am here!\n");
				break; 
			} 
			case OPCODE_areturn: //throws IllegalMonitorStateException
			{ 
				STACK_POP(m1); //objectref
				RETURN_FROM_METHOD(m1);
				// TODO: Add IllegalMonitorStateException
				break;
			} 
			case OPCODE_arraylength: //throws NullPointerException
			{ 
				STACK_POP(m1); //arrayref
				if(((J_reference_array*) m1) == J_null) THROW_EXCEPTION("NullPointerException");
				STACK_PUSH( &(((J_reference_array*) m1)->length)); //length
				break;
			} 
			case OPCODE_astore:
			{ 
				CODE_READ(b1); //index
				STACK_POP(m1); //objectref
				locals[b1] = m1;
				break;
			} 
			case OPCODE_astore_0:
			{ 
				STACK_POP(m1); //objectref
				locals[0] = m1;
				break;
			} 
			case OPCODE_astore_1:
			{ 
				STACK_POP(m1); //objectref
				locals[1] = m1;
				break;
			} 
			case OPCODE_astore_2:
			{ 
				STACK_POP(m1); //objectref
				locals[2] = m1;
				break;
			} 
			case OPCODE_astore_3:
			{ 
				STACK_POP(m1); //objectref
				locals[3] = m1;
				break;
			} 
			case OPCODE_athrow: //throws NullPointerException, IllegalMonitorStateException
			{ 
				STACK_POP(m1); //objectref
				if(m1 == J_null) THROW_EXCEPTION("NullPointerException");
				THROW_EXCEPTION(m1); //TODO: Throw exception and do other stuff
				break;
			} 
			case OPCODE_baload: // throws NullPointerException, ArrayIndexOutOfBoundsException
			{ 
				STACK_POP(m2); //index
				STACK_POP(m1); //arrayref
				if(((J_reference_array*) m1) == J_null) THROW_EXCEPTION("NullPointerException");
				if( (*((J_int*) m2)) >= ((J_reference_array*)m1)->length) THROW_EXCEPTION("ArrayIndexOutOfBoundsException");
				m3 = ((J_reference_array*) m1)->array[*((J_int*) m2)];
				STACK_PUSH(m3); //value
				break;
			} 
			case OPCODE_bastore: // throws NullPointerException, ArrayIndexOutOfBoundsException
			{ 
				STACK_POP(m3); //value
				STACK_POP(m2); //index
				STACK_POP(m1); //arrayref
				if(((J_reference_array*) m1) == J_null) THROW_EXCEPTION("NullPointerException");
				if( (*((J_int*) m2)) >= ((J_reference_array*)m1)->length) THROW_EXCEPTION("ArrayIndexOutOfBoundsException");
				((J_reference_array*) m1)->array[*((J_int*) m2)] = (m3 + 3); // FIXME: Check if this works (int to byte)
				break;
			} 
			case OPCODE_bipush: 
			{ 
				CODE_READ(b1); //byte
				m3 = (void*) malloc(1);
				((u8int*)m3)[0] = b1;
				STACK_PUSH(m3);
				
				break;
			} 
			case OPCODE_caload: // throws NullPointerException, ArrayIndexOutOfBoundsException
			{ 
				STACK_POP(m2); //index
				STACK_POP(m1); //arrayref

				if(((J_reference_array*) m1) == J_null) THROW_EXCEPTION("NullPointerException");
				if( (*((J_int*) m2)) >= ((J_reference_array*)m1)->length) THROW_EXCEPTION("ArrayIndexOutOfBoundsException");
				STACK_PUSH(((J_reference_array*) m1)->array[(*((J_int*) m2))]); 
				break;
			} 
			case OPCODE_castore: // throws NullPointerException, ArrayIndexOutOfBoundsException
			{ 
				STACK_POP(m3); //value
				STACK_POP(m2); //index
				STACK_POP(m1); //arrayref
				if(((J_reference_array*) m1) == J_null) THROW_EXCEPTION("NullPointerException");
				if( (*((J_int*) m2)) >= ((J_reference_array*)m1)->length) THROW_EXCEPTION("ArrayIndexOutOfBoundsException");
				((J_reference_array*) m1)->array[*((J_int*) m2)] = (m3 + 2); // FIXME: Check if this works (int to char)
				break;
			} 
			case OPCODE_checkcast: // throws ClassCastException
			{ 
				CODE_READ(b1); //indexbyte1
				CODE_READ(b2); //indexbyte2				
				STACK_POP(m1); //objectref
				if(( (J_reference*) m1) == J_null) 
				{
					STACK_PUSH(m1);
					break;
				}

				b3 = g_jcf->cp[((b1 << 8) | b2)]->tag; //tag
				if(b3 == CONSTANT_Class) // a class or interface
				{
					m2 = (void*) ((CONSTANT_Utf8_info*) g_jcf->cp[MAKE_U16(((CONSTANT_Class_info*) g_jcf->cp[((b1 << 8) | b2)]->data)->name_index)]->data)->bytes; //classname
					if(m2[0]!='[') //if not an array
					{

					}
				}
				else if(b3 == CONSTANT_NameAndType) //
				{

				} // NOTE: There might be more possibilities
				STACK_PUSH(m1); //objectref
				break;
			} 
			case OPCODE_dadd:
			{ 
				break;
			} 
			case OPCODE_daload:
			{ 
				break;
			} 
			case OPCODE_dastore:
			{ 
				break;
			} 
			case OPCODE_dcmpg:
			{ 
				break;
			} 
			case OPCODE_dcmpl:
			{ 
				break;
			} 
			case OPCODE_dconst_0:
			{ 
				break;
			} 
			case OPCODE_dconst_1:
			{ 
				break;
			} 
			case OPCODE_ddiv:
			{ 
				break;
			} 
			case OPCODE_dload:
			{ 
				break;
			} 
			case OPCODE_dload_0:
			{ 
				break;
			} 
			case OPCODE_dload_1:
			{ 
				break;
			} 
			case OPCODE_dload_2:
			{ 
				break;
			} 
			case OPCODE_dload_3:
			{ 
				break;
			} 
			case OPCODE_dmul:
			{ 
				break;
			} 
			case OPCODE_dneg:
			{ 
				break;
			} 
			case OPCODE_drem:
			{ 
				break;
			} 
			case OPCODE_dreturn:
			{ 
				break;
			} 
			case OPCODE_dstore:
			{ 
				break;
			} 
			case OPCODE_dstore_0:
			{ 
				break;
			} 
			case OPCODE_dstore_1:
			{ 
				break;
			} 
			case OPCODE_dstore_2:
			{ 
				break;
			} 
			case OPCODE_dstore_3:
			{ 
				break;
			} 
			case OPCODE_dsub:
			{ 
				break;
			} 
			case OPCODE_dup:
			{ 
				break;
			} 
			case OPCODE_dup_x1:
			{ 
				break;
			} 
			case OPCODE_dub_x2:
			{ 
				break;
			} 
			case OPCODE_dup2:
			{ 
				break;
			} 
			case OPCODE_dup2_x1:
			{ 
				break;
			} 
			case OPCODE_dup2_x2:
			{ 
				break;
			} 
			case OPCODE_fadd:
			{ 
				break;
			} 
			case OPCODE_faload:
			{ 
				break;
			} 
			case OPCODE_fastore:
			{ 
				break;
			} 
			case OPCODE_fcmpg:
			{ 
				break;
			} 
			case OPCODE_fcmpl:
			{ 
				break;
			} 
			case OPCODE_fconst_0:
			{ 
				break;
			} 
			case OPCODE_fconst_1:
			{ 
				break;
			} 
			case OPCODE_fconst_2:
			{ 
				break;
			} 
			case OPCODE_fdiv:
			{ 
				break;
			} 
			case OPCODE_fload:
			{ 
				break;
			} 
			case OPCODE_fload_0:
			{ 
				break;
			} 
			case OPCODE_fload_1:
			{ 
				break;
			} 
			case OPCODE_fload_2:
			{ 
				break;
			} 
			case OPCODE_float_3:
			{ 
				break;
			} 
			case OPCODE_fmul:
			{ 
				break;
			} 
			case OPCODE_fneg:
			{ 
				break;
			} 
			case OPCODE_frem:
			{ 
				break;
			} 
			case OPCODE_freturn:
			{ 
				break;
			} 
			case OPCODE_fstore:
			{ 
				break;
			} 
			case OPCODE_fstore_0:
			{ 
				break;
			} 
			case OPCODE_fstore_1:
			{ 
				break;
			} 
			case OPCODE_fstore_2:
			{ 
				break;
			} 
			case OPCODE_fstore_3:
			{ 
				break;
			} 
			case OPCODE_fsub:
			{ 
				break;
			} 
			case OPCODE_getfield:
			{ 
				break;
			} 
			case OPCODE_getstatic:
			{ 
				break;
			} 
			case OPCODE_goto:
			{ 
				break;
			} 
			case OPCODE_goto_w:
			{ 
				break;
			} 
			case OPCODE_i2l:
			{ 
				break;
			} 
			case OPCODE_i2f:
			{ 
				break;
			} 
			case OPCODE_i2d:
			{ 
				break;
			} 
			case OPCODE_l2i:
			{ 
				break;
			} 
			case OPCODE_l2f:
			{ 
				break;
			} 
			case OPCODE_l2d:
			{ 
				break;
			} 
			case OPCODE_f2i:
			{ 
				break;
			} 
			case OPCODE_f2l:
			{ 
				break;
			} 
			case OPCODE_f2d:
			{ 
				break;
			} 
			case OPCODE_d2i:
			{ 
				break;
			} 
			case OPCODE_d2l:
			{ 
				break;
			} 
			case OPCODE_d2f:
			{ 
				break;
			} 
			case OPCODE_i2b:
			{ 
				break;
			} 
			case OPCODE_i2c:
			{ 
				break;
			} 
			case OPCODE_i2s:
			{ 
				break;
			} 
			case OPCODE_iadd:
			{ 
				break;
			} 
			case OPCODE_iaload:
			{ 
				break;
			} 
			case OPCODE_iand:
			{ 
				break;
			} 
			case OPCODE_iastore:
			{ 
				break;
			} 
			case OPCODE_iconst_m1:
			{ 
				break;
			} 
			case OPCODE_iconst_0:
			{ 
				break;
			} 
			case OPCODE_iconst_1:
			{ 
				break;
			} 
			case OPCODE_iconst_2:
			{ 
				break;
			} 
			case OPCODE_iconst_3:
			{ 
				break;
			} 
			case OPCODE_iconst_4:
			{ 
				break;
			} 
			case OPCODE_iconst_5:
			{ 
				break;
			} 
			case OPCODE_idiv:
			{ 
				break;
			} 
			case OPCODE_if_acmpeq:
			{ 
				break;
			} 
			case OPCODE_if_acmpne:
			{ 
				break;
			} 
			case OPCODE_if_icmpeq:
			{ 
				break;
			} 
			case OPCODE_if_icmpne:
			{ 
				break;
			} 
			case OPCODE_if_icmplt:
			{ 
				break;
			} 
			case OPCODE_if_icmpge:
			{ 
				break;
			} 
			case OPCODE_if_icmpgt:
			{ 
				break;
			} 
			case OPCODE_if_icmple:
			{ 
				break;
			} 
			case OPCODE_ifeq:
			{ 
				break;
			} 
			case OPCODE_ifne:
			{ 
				break;
			} 
			case OPCODE_iflt:
			{ 
				break;
			} 
			case OPCODE_ifge:
			{ 
				break;
			} 
			case OPCODE_ifgt:
			{ 
				break;
			} 
			case OPCODE_ifle:
			{ 
				break;
			} 
			case OPCODE_ifnonnull:
			{ 
				break;
			} 
			case OPCODE_ifnull:
			{ 
				break;
			} 
			case OPCODE_iinc:
			{ 
				break;
			} 
			case OPCODE_iload:
			{ 
				break;
			} 
			case OPCODE_iload_0:
			{ 
				break;
			} 
			case OPCODE_iload_1:
			{ 
				break;
			} 
			case OPCODE_iload_2:
			{ 
				break;
			} 
			case OPCODE_iload_3:
			{ 
				break;
			} 
			case OPCODE_imul:
			{ 
				break;
			} 
			case OPCODE_ineg:
			{ 
				break;
			} 
			case OPCODE_instanceof:
			{ 
				break;
			} 
			case OPCODE_invokedynamic:
			{ 
				break;
			} 
			case OPCODE_invokeinterface:
			{ 
				break;
			} 
			case OPCODE_invokespecial:
			{ 
				break;
			} 
			case OPCODE_invokestatic:
			{ 
				break;
			} 
			case OPCODE_invokevirtual:
			{ 
				break;
			} 
			case OPCODE_ior:
			{ 
				break;
			} 
			case OPCODE_irem:
			{ 
				break;
			} 
			case OPCODE_ireturn:
			{ 
				break;
			} 
			case OPCODE_ishl:
			{ 
				break;
			} 
			case OPCODE_ishr:
			{ 
				break;
			} 
			case OPCODE_istore:
			{ 
				break;
			} 
			case OPCODE_istore_0:
			{ 
				break;
			} 
			case OPCODE_istore_1:
			{ 
				break;
			} 
			case OPCODE_istore_2:
			{ 
				break;
			} 
			case OPCODE_istore_3:
			{ 
				break;
			} 
			case OPCODE_isub:
			{ 
				break;
			} 
			case OPCODE_iushr:
			{ 
				break;
			} 
			case OPCODE_ixor:
			{ 
				break;
			} 
			case OPCODE_jsr:
			{ 
				break;
			} 
			case OPCODE_jsr_w:
			{ 
				break;
			} 
			case OPCODE_lad:
			{ 
				break;
			} 
			case OPCODE_laload:
			{ 
				break;
			} 
			case OPCODE_land:
			{ 
				break;
			} 
			case OPCODE_lastore:
			{ 
				break;
			} 
			case OPCODE_lcmp:
			{ 
				break;
			} 
			case OPCODE_lconst_0:
			{ 
				break;
			} 
			case OPCODE_lconst_1:
			{ 
				break;
			} 
			case OPCODE_ldc:
			{ 
				break;
			} 
			case OPCODE_ldc_w:
			{ 
				break;
			} 
			case OPCODE_ldc2_w:
			{ 
				break;
			} 
			case OPCODE_ldiv:
			{ 
				break;
			} 
			case OPCODE_lload:
			{ 
				break;
			} 
			case OPCODE_lload_0:
			{ 
				break;
			} 
			case OPCODE_lload_1:
			{ 
				break;
			} 
			case OPCODE_lload_2:
			{ 
				break;
			} 
			case OPCODE_lload_3:
			{ 
				break;
			} 
			case OPCODE_lmul:
			{ 
				break;
			} 
			case OPCODE_lneg:
			{ 
				break;
			} 
			case OPCODE_lookupswitch:
			{ 
				break;
			} 
			case OPCODE_lor:
			{ 
				break;
			} 
			case OPCODE_lrem:
			{ 
				break;
			} 
			case OPCODE_lreturn:
			{ 
				break;
			} 
			case OPCODE_lshl:
			{ 
				break;
			} 
			case OPCODE_lshr:
			{ 
				break;
			} 
			case OPCODE_lstore:
			{ 
				break;
			} 
			case OPCODE_lstore_0:
			{ 
				break;
			} 
			case OPCODE_lstore_1:
			{ 
				break;
			} 
			case OPCODE_lstore_2:
			{ 
				break;
			} 
			case OPCODE_lstore_3:
			{ 
				break;
			} 
			case OPCODE_lsub:
			{ 
				break;
			} 
			case OPCODE_lushr:
			{ 
				break;
			} 
			case OPCODE_lxor:
			{ 
				break;
			} 
			case OPCODE_monitorenter:
			{ 
				break;
			} 
			case OPCODE_monitorexit:
			{ 
				break;
			} 
			case OPCODE_multianewarray:
			{ 
				break;
			} 
			case OPCODE_new:
			{ 
				break;
			} 
			case OPCODE_newarray:
			{ 
				break;
			} 
			case OPCODE_nop:
			{ 
				break;
			} 
			case OPCODE_pop:
			{ 
				break;
			} 
			case OPCODE_pop2:
			{ 
				break;
			} 
			case OPCODE_putfield:
			{ 
				break;
			} 
			case OPCODE_putstatic:
			{ 
				break;
			} 
			case OPCODE_ret:
			{ 
				break;
			} 
			case OPCODE_return:
			{ 
				break;
			} 
			case OPCODE_saload:
			{ 
				break;
			} 
			case OPCODE_sastore:
			{ 
				break;
			} 
			case OPCODE_sipush:
			{ 
				break;
			} 
			case OPCODE_swap:
			{ 
				break;
			} 
			case OPCODE_tableswitch:
			{ 
				break;
			} 
			case OPCODE_wide:
			{ 
				break;
			} 
			case OPCODE_breakpoint:
			{ 
				break;
			} 
			case OPCODE_impdep1:
			{ 
				break;
			} 
			case OPCODE_impdep2:
			{ 
				break;
			} 
			default:
			{
				debugerr("Invalid opcode: 0x%02X\n", opcode);
				return -E_OPCODE;
				
			}
		}
	}
	
	 
	
     return 0;
}
s8int HjvmExecute(JavaClassFile *jcf, char* methodname, u16int access_flags, u8int argc, void **argv, char* argformat, void *returnvalue)
 {
     if(jcf==NULL) return -E_NULL_POINTER;  
     if(jcf->methods_count < 1) return -E_METHOD_NOT_FOUND;
   
     g_jcf = jcf;
	
     u16int i,j;
     u16int method_index=0;
     u16int name_index;
     u16int descriptor_index;
     char* method_name;
     char* descriptor = NULL;
     char* arg_descriptor; 
     
     /* Find the right method */ 
     for(i=0; i < jcf->methods_count; i++)
     {
        name_index = jcf->methods[i]->name_index;
        method_name = (char*) ((CONSTANT_Utf8_info*) jcf->cp[name_index-1]->data)->bytes;
        if(strcmp(methodname, method_name)==0)
        {
              
               descriptor_index = jcf->methods[i]->descriptor_index;
               descriptor = (char*) ((CONSTANT_Utf8_info*) jcf->cp[descriptor_index-1]->data)->bytes;
               
               
               for(j=1;j< MAKE_U16(((CONSTANT_Utf8_info*)jcf->cp[descriptor_index-1]->data)->length); j++)
               {
                    if(descriptor[j]==')')
                    {
                         arg_descriptor = substring(descriptor, 1, j);
                         break;
                    }
               }
               if(strcmp(arg_descriptor, argformat)!=0) continue;
               else
               {
                    method_index=i;
                    break;
               }
          }
         
     }
     /* If the right method is not found */
     if(descriptor==NULL)
     {
          verbosemsg("Can\'t find method \'%s\' in class %s!\n", methodname, ((CONSTANT_Utf8_info*)jcf->cp[MAKE_U16(((CONSTANT_Class_info*)jcf->cp[jcf->this_class-1]->data)->name_index)-1]->data)->bytes);
		
		//verbosemsg("This class: %s\n", );
          return -E_METHOD_NOT_FOUND;
          
     }
     /* Check if method is native or abstract */
     if(jcf->methods[method_index]->access_flags & METHOD_ACC_ABSTRACT)
           return -E_ABSTRACT;
     else if (jcf->methods[method_index]->access_flags & METHOD_ACC_NATIVE)
     { return -E_ABSTRACT;} //TODO: Execute the native method inside the VM.
         
          
          
     /* Get the Code attribute of the method */
     // TODO: Add exceptions 
     
     Code_attribute* code;
     attribute_info* attribute;
     char* attribute_name;
     for(i=0;i<jcf->methods[method_index]->attributes_count;i++)
     {
          attribute = jcf->methods[method_index]->attributes[i];
          attribute_name = (char*) ((CONSTANT_Utf8_info*) jcf->cp[attribute->attribute_name_index-1]->data)->bytes;
          if(strcmp(attribute_name, "Code")==0) {
               
               code = attribute_to_code(attribute);
               break;
          }
     }
     
     /* Execute the code */
     return HjvmExecuteCode(code, NULL, argc, argv, returnvalue);
     
 }