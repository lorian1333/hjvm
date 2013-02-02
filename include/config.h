#ifndef CONFIG_H
#define CONFIG_H
typedef signed char s8int;
typedef unsigned char u8int;
typedef signed short s16int;
typedef unsigned short u16int;
typedef signed int s32int;
typedef unsigned int u32int;
typedef signed long long s64int;
typedef unsigned long long u64int;
typedef float float32;
typedef double float64;

#define MIN_CLASS_VERSION JDK1_1

#ifndef MANUAL_ENDIANNESS
#define LITTLE_ENDIAN_TEST 0x41424344UL 
#define BIG_ENDIAN_TEST    0x44434241UL
#define PDP_ENDIAN_TEST    0x42414443UL
#define ENDIAN_ORDER  ('ABCD') 


#if ENDIAN_ORDER==LITTLE_ENDIAN_TEST
#define SYSTEM_LITTLE_ENDIAN
#elif ENDIAN_ORDER==BIG_ENDIAN_TEST
#define SYSTEM_BIG_ENDIAN
#elif ENDIAN_ORDER==PDP_ENDIAN_TEST
#error "BDP Endianness not supported"
#else
#error "Unrecognized endianness. Please #define MANUAL_ENDIANNESS and #define SYSTEM_LITTLE_ENDIAN or SYSTEM_BIG_ENDIAN."
#endif // endianness test
#endif //!MANUAL_ENDIANNESS

#ifdef SYSTEM_BIG_ENDIAN
#define makeRGB(r,g,b) (r * 0x10000 | g * 0x100 | b)
#elif defined SYSTEM_LITTLE_ENDIAN
#define makeRGB(r,g,b) (b * 0x10000 | g * 0x100 | r)

#elif defined MANUAL_ENDIANNESS
#error "You defined MANUAL_ENDIANNESS but didn't define SYSTEM_LITTLE_ENDIAN or SYSTEM_BIG_ENDIAN."
#else
#error "SYSTEM_LITTLE_ENDIAN or SYSTEM_BIG_ENDIAN must be defined automatically."
#endif //SYSTEM_BIG_ENDIANNESS



#define MAKE_U16(b) ( (b[0] * 0x100) | b[1] )
#define MAKE_U32(b) ( (b[0] * 0x10000) | (b[1] * 0x1000) | (b[2] * 0x100) | b[3])
//#define MAKE_S64(h, l) ( h[0] * 0x1000000000000 | h[1] * 0x1000000000000 |h[2] * 0x10000000000 | h[3] * 0x100000000 | l[0] * 0x1000000 | l[1] * 0x10000 | l[2] * 0x100 | l[3]) 
#ifdef SYSTEM_LITTLE_ENDIAN
#define MAKE_S32(a, b) { a[0]=b[3]; a[1]=b[2]; a[2]=b[1], a[3]=b[0]; }
#define MAKE_S64(a, b) { a[0]=b[7]; a[1]=b[6]; a[2]=b[5], a[3]=b[4]; a[4]=b[3]; a[5]=b[2]; a[6]=b[1], a[7]=b[0];  }
#else
#define MAKE_S32(a, b) { a[0]=b[0]; a[1]=b[1]; a[2]=b[2]; a[3]=b[3]; }
#define MAKE_S64(a, b) { a[0]=b[0]; a[1]=b[1]; a[2]=b[2], a[3]=b[3]; a[4]=b[4]; a[5]=b[5]; a[6]=b[6], a[7]=b[7];  }
#endif

#define COPY_FLOAT(a, b) MAKE_S32(a, b)
#define COPY_DOUBLE(a, b) MAKE_S64(a, b)

#define READ_U2(d, f) { d[0] = fgetc(f); d[1] = fgetc(f); }
#define READ_U4(d, f) { d[0] = fgetc(f); d[1] = fgetc(f); d[2] = fgetc(f); d[3] = fgetc(f); }
 


extern const char* hjvm_executable_name;

#define hjvm_be_verbose() (hjvm_verbose_level()>0)
#define hjvm_be_extra_verbose() (hjvm_verbose_level()>1)
#define verbosemsg(...) if(hjvm_be_verbose()) { fprintf(stdout, "%s: ", hjvm_executable_name); fprintf(stdout, __VA_ARGS__); }
#define debugmsg(...) if(hjvm_be_extra_verbose()) { fprintf(stdout, "%s: DEBUG: ", hjvm_executable_name); fprintf(stdout, __VA_ARGS__); }
#define debugerr(...) if(hjvm_be_verbose()) { fprintf(stdout, "%s: DEBUG ERROR: ", hjvm_executable_name); fprintf(stdout, __VA_ARGS__); }
#define err(...) { fprintf(stdout, "%s: error: ", hjvm_executable_name); fprintf(stdout, __VA_ARGS__);  }
#define errwithmsg(...) { fprintf(stdout, "%s: error: ", hjvm_executable_name); fprintf(stdout, __VA_ARGS__); printf(": "); perror(""); }

#endif
