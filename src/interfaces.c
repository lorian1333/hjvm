#include <hjvm.h>


int read_interfaces(FILE* f, u16int interfaces_count, interface_info** interfaceinfo)
{
     int i;
     u8int* bytes = (u8int*) malloc(2);
     interface_info* info;
     debugmsg("Interfaces:\n");
     for(i=0;i<interfaces_count;i++)
     {
          info = (interface_info*) malloc(sizeof(interface_info));
          READ_U2(bytes, f);
          info->index = MAKE_U16(bytes);
          //debugmsg("#%d #%d\n", (i+1), info->index);
          interfaceinfo[i] = info;
     }
     return 1;
}
