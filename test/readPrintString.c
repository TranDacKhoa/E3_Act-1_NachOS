#include "syscall.h"

int
main()
{
    char str[256];
    PrintString("vui lòng nhập chiều dài chuỗi: (tối đa 255):\n");
    ReadString(str,ReadNum()); 
    PrintString(str);

    Halt();
  /* not reached */
}
