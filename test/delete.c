#include "syscall.h"
int main()
{
    char name[256];
    int check;
    int res;
    char buffer[100];
    PrintString("Nhập tên file: \n");
    ReadString(name, 255);
    check = Open(name, 1); // check = fileID
    Open("b.txt",1);
    Close(check);
    if (check == -1)
    {
        PrintNum(check);
        PrintString("không tồn tại file: \n");
    }
    else
    {
        PrintNum(check);

        if (RemoveFile(name) == 0)
        {
            PrintString(" removed successfully!\n");
        }
        else
            PrintString("Remove file failed\n");
    }

   
    Halt();
}