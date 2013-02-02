#include <hjvm.h>
#include <jcf.h>
#include <time.h>

/*
References:
     http://docs.oracle.com/javase/specs/jvms/se5.0/html/VMSpecTOC.doc.html - Java Virtual Machine Specifications (Second Edition)
     http://docs.oracle.com/javase/specs/jvms/se7/html/ - Java Virtual Machine Specifications (Java SE 7 Edition) 
     http://www.cdrummond.qc.ca/cegep/informat/professeurs/alain/files/ascii.htm - Extended ASCII Table
     http://www.utf8-chartable.de/ - UTF-8 encoding table

*/

int be_verbose=0;
int be_extra_verbose;

const char* hjvm_executable_name;
#define EARG -1

void showhelp()
{
	printf("Usage: %s [options] classfile [args]\n", hjvm_executable_name);
	puts("Options:");
     puts("   --help                            Show this help");
	puts("   -v, --verbose                     Be verbose"    );
     puts("   -xv, --extra-verbose              Be -extra- verbose. Enable debug messages"    );
     puts("   -c (dir), --classpath (dir)       Search for classfiles in this directory\n"
          "                                     and its subdirectories"    );
     puts("   -z (dir), --zip-archive (dir)     Search for classfiles in this ZIP archive");
     puts("");
 }
 char ** classpaths;
 u16int classpaths_count;
 char ** ziparchives;
 u16int ziparchives_count;
 char* mainfile;
 u16int main_argc=0;
 char** main_argv;
 
int main(int argc, char *argv[])
{
     clock_t t1, t2;   
     t1 = clock();   
	hjvm_executable_name = argv[0];
     classpaths = (char**) malloc(100);
     memset(classpaths, 0, 100);
     ziparchives = (char**) malloc(100);
     memset(ziparchives, 0, 100);
	int i;
      main_argv = (char**) malloc(50 * sizeof(char*));
	if(argc==1) 
     {
		err("No input files. \nType --help for help.\n");
          return -EARG;
     }
     for(i=1;i<argc;i++)
     {
          if(strcmp(argv[i], "--help")==0)
               {
                    showhelp();
                    return 0;
               }
      }   
      u8int arg_index=0;
      
	for(i=1;i<argc;i++)
	{
		if(strcmp(argv[i], "--help")==0)
		{
			showhelp();
			return 0;
		}
          else if(strcmp(argv[i], "-v")==0 || strcmp(argv[i],"--verbose")==0)
			be_verbose=1;
           else if(strcmp(argv[i], "-xv")==0 || strcmp(argv[i],"--extra-verbose")==0)
           {
               be_verbose=1;
			be_extra_verbose=1;
           }
          else if(strcmp(argv[i], "-c")==0 || strcmp(argv[i], "--classpath")==0)
          {
               if(i+1 == argc-1) 
               {
                    err("%s should be followed by a directory. \nType --help for help.\n", argv[i]); 
                    return -EARG;
               }
               classpaths[classpaths_count++] = argv[++i];
          }
          else if(strcmp(argv[i], "-z")==0 || strcmp(argv[i], "--zip-archive")==0)
          {
               if(i+1 == argc-1) 
               {
                    err("%s should be followed by a path to a ZIP archive. \nType --help for help.\n", argv[i]);
                    return -EARG;
               }
               ziparchives[ziparchives_count++] = argv[++i];
          }
          else //assume file is found
          {
               //err("Unrecognized command line option: \'%s\'. \nType --help for help.\n", argv[i]);
               //return -EARG;
               arg_index = i;
               mainfile = argv[i];
               break;
          }
          
	}
     
     char* message = (char*) malloc(100);
     memset(message, 0, 100);
     strcpy(message, "Main arguments: ");
     for(i=arg_index+1;i<argc;i++)
     {
          main_argv[main_argc++] = argv[i];
          sprintf(message, "%s %s", message, argv[i]);
          
     }
     
     verbosemsg("Current working directory: %s\n", getcwd(NULL, 1024));
    
    
     if(hjvm_be_verbose())
     {    
          if(classpaths_count>0) {
               verbosemsg("Classpaths included:\n");
               for(i=0;i<classpaths_count;i++)
                    verbosemsg("%s\n", classpaths[i]);
     
          }
          if(ziparchives_count>0) {
               verbosemsg("ZIP archives included:\n");
               for(i=0;i<ziparchives_count;i++)
                    verbosemsg("%s\n", ziparchives[i]);
          }
     }
     
     if(main_argc > 0) verbosemsg("%s\n", message);
     free(message);
     
	JavaClassFile* jcf = HjvmLoad(mainfile); 
     if(jcf==NULL)
     {
          err("Invalid class file.\n");
     }
     verbosemsg("Executing main method\n");
     HjvmExecute(jcf, "main", (METHOD_ACC_PUBLIC | METHOD_ACC_STATIC), main_argc, (void**) main_argv, "[Ljava/lang/String;", NULL);
     
     //Sleep(1000);
     t2 = clock();   
     float diff = (((float)t2 - (float)t1) / 1000000.0F ) * 1000;   
     printf("Execution took approximately %f seconds\n", diff);
     
	return 0;
}

int hjvm_verbose_level(void)
{
	return be_verbose+be_extra_verbose;
}


