#include "syscall.h"

int
main()
{
    
    PrintString("Gioi thieu ve nhom\n");
    PrintString("20120511 - Nguyen Quoc Khoa\n");
    PrintString("20120513 - Nguyen Dac Khoa\n");
    PrintString("20120506 - Tran Duy Khang\n");

    // Mo ta chuong trinh Sort
    PrintString(
        "Sort: tai thu muc test thuc hien cau lenh: ../build.linux/nachos -x sort\n");
    PrintString("Bat dau thuc thi chuong trinh\n");
    PrintString("Nhap so luong phan tu mang: (n<100)\n");
    PrintString("Nhap gia tri moi phan tu\n");
    PrintString("Lua chon: 0 - sap xep tang dan, 1 - sap xep giam dan\../build.linux/nachos -x readPrintChar -d un");
    PrintString("Thuc hien sap xep va in mang ket qua\n");
    PrintString("Ket thuc chuong trinh\n");


    // Mo ta chuong trinh xuat ASCII
    PrintString(
        "ASCII: tai thu muc test thuc hien cau lenh: ../build.linux/nachos -x ascii\n");
    PrintString("In ket qua\n");
    PrintString("Ket thuc chuong trinh\n");



    Halt();
  /* not reached */
}
