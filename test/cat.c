#include "syscall.h"
int main(){
    char name[256];
    int check;
    int res ;
    char buffer[100];
    PrintString("Nhập tên file: \n");
    ReadString(name,255); 
    check=Open(name,1);
    if(check==-1){
        PrintString("không tồn tại file: \n");
    }
    else{
        res= Read(buffer,50,check);
    }

    if(res==-1){
        PrintString("đọc file không thành công:\n");
    }
    else{
        PrintString(buffer);
        PrintString("\n");
    }
 
    Close(check);
    Halt();
};