#include "syscall.h"

int main() {

    char *filename[100];

    ReadString(filename,255); 
    if (CreateFile(filename) == 0) {
        // PrintString("File ");
        // PrintString(fileName);
        PrintString(" created successfully!\n");
    } else
        PrintString("Create file failed\n");

    Halt();
}
