/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "fileDescriptor.h"

#define FILE_MAX 10
#define CONSOLE_IN 0
#define CONSOLE_OUT 1
#define MODE_READWRITE 0
#define MODE_READ 1
#define MODE_WRITE 2

//khai báo mảng fileDEscriptorTable
file **fileDescriptorTable= new file*[FILE_MAX];
int *fileOpenMode = new int[FILE_MAX];


#include "kernel.h"
#include "synchconsole.h"
#include <time.h>

#define INT32_MIN (-2147483647 - 1)
#define INT32_MAX 2147483647

/* độ dài tối đa của một số kiểu dữ liệu integer 32 bit (bao gồm dấu -)*/
#define MAX_INT32_LENGTH 11

char buffer[MAX_INT32_LENGTH + 2];
void SysHalt()
{
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

char isBlank(char c)
{
    if (c == (char)10 || c == (char)13 || c == (char)9 || c == ' ') // line break/ new line/ tab/ space
        return 1;
    return 0;
}

void readInputNum()
{
    memset(buffer, 0, sizeof(buffer));
    char c = kernel->synchConsoleIn->GetChar();

    // nếu chỉ nhập khoảng trắng hoặc các tab rồi bấm enter thì vẫn cho phép nhập tiếp
    while (isBlank(c))
    {
        c = kernel->synchConsoleIn->GetChar();
    }

    // khi gặp kí tự không phải là khoảng trắng hay tab thì ta bắt đầu xét chuỗi đọc vào
    int index = 0;
    while (!(isBlank(c) || c == EOF))
    {

        if (index <= MAX_INT32_LENGTH)
        {
            buffer[index] = c;
            index++;
        }
        c = kernel->synchConsoleIn->GetChar();
    }
    if (index > MAX_INT32_LENGTH)
    {
        DEBUG(dbgSys, "số nhập vào quá dài, xảy ra tràn số");
    }
}

bool isOverflow(int integer, const char *s)
{
    if (integer == 0)
        return strcmp(s, "0") == 0;

    int len = strlen(s);

    if (integer < 0 && s[0] != '-')
    {
        return true;
    }

    if (integer < 0)
    {
        integer = -integer;
    }

    while (integer > 0)
    {
        int digit = integer % 10;

        if (s[len - 1] - '0' != digit)
            return true;

        --len;
        integer /= 10;
    }

    return false;
}

int SysReadNum()
{
    readInputNum();

    int len = strlen(buffer);
    // vì range của integer là từ -2147483648 đến 2147483647
    // mà chúng ta xử lý trị tuyệt đối của số trước rồi sau đó mới xét dấu rồi add vào
    // nên chúng ta phải xét trường hợp số đạt INT_MIN trước. Chứ không sẽ bị tràn số.
    if (strcmp(buffer, "-2147483648") == 0)
        return INT32_MIN;
    // dấu của số nhập vào, mặc định là số dương
    bool sign = 0;
    // nếu kí tự đầu là dấu âm thì gán sign=1
    if (buffer[0] == '-')
    {
        sign = 1;
    }
    int num = 0;
    for (int i = sign; i < len; ++i)
    {
        char c = buffer[i];

        if (c < '0' || c > '9')
        {
            DEBUG(dbgSys, "Tìm thấy kí tự không phải số " << buffer);
            return 0;
        }
        num = num * 10 + (c - '0');
    }

    // nếu là dấu âm thì là trúng ta gán num bằng -num
    if (sign == 1)
        num = -num;

    // kiem tra tran so
    if (!isOverflow(num, buffer))
        return num;
    else
        DEBUG(dbgSys, "Tràn số");

    return 0;
}

void SysPrintNum(int num)
{
    if (num == 0)
    {
        return kernel->synchConsoleOut->PutChar('0');
    }
    // nếu là số âm bé nhất của 32bit integer thì chúng ta sẽ in riêng.

    if (num == INT32_MIN)
    {
        char int32_min[10] = {'2', '1', '4', '7', '4', '8', '3', '6', '4', '8'};
        kernel->synchConsoleOut->PutChar('-');
        for (int i = 0; i < 10; ++i)
            kernel->synchConsoleOut->PutChar(int32_min[i]);
        return;
    }

    // nếu là số âm thì in ra dấu - trước
    if (num < 0)
    {
        kernel->synchConsoleOut->PutChar('-');
        num = -num;
    }
    int index = 0;
    // tạo một chuỗi theo thứ tự ngược lại với giá trị cần in
    while (num)
    {
        buffer[index] = num % 10;
        index++;
        num = num / 10;
    }
    // in phần trị tuyệt đối của số
    // chúng ta in ngược chuỗi đã tạo.
    for (int i = index - 1; i >= 0; --i)
        kernel->synchConsoleOut->PutChar(buffer[i] + '0');
}

int SysRandom()
{
    srand(time(NULL));
    return 1 + rand() % 2147483647;
}

char SysReadChar()
{

    return kernel->synchConsoleIn->GetChar();
}

void SysPrintChar(char character)
{
    kernel->synchConsoleOut->PutChar(character);
}

char *SysReadString(int length)
{
    char str_temp[100];
    int i = 0;

    while (1)
    {

        str_temp[i] = SysReadChar();
        if (str_temp[i] == char(10)) // char(10) linebreak
        {
            break;
        }

        i++;
    }
    str_temp[i + 1] = '\0';
    int len = strlen(str_temp);
    if ((len - 1) > length)
    {
        return NULL;
    }
    char *buffer = new char[len];
    for (int i = 0; i < len; i++)
    {
        buffer[i] = str_temp[i];
    }
    buffer[len - 1] = '\0';
    return buffer;
}
void SysPrintString(char *str)
{

    int length = strlen(str);
    for (int i = 0; i < length; i++)
    {
        SysPrintChar(str[i]);
    }
}

/**/
bool SysCreateFile(char *nameFile)
{
    bool result;

    int length = strlen(nameFile);
    DEBUG(dbgSys, length);
    //nếu tên file rỗng return false 
    if (length == 0)
    {
        DEBUG(dbgSys, "\nFile name can't be empty");
        result = false;
    } 
    else if (nameFile == NULL) //nếu tên file bằng NULL thì do hệ điều hành không còn đủ bộ nhớ
    {
        DEBUG(dbgSys, "\nNot enough memory in system");
        result = false;
    }
    else
    {
        //gọi create thông qua lớp fileSystem
        if (!kernel->fileSystem->Create(nameFile)) 
        {
            DEBUG(dbgSys, "\nCreate Failed");
            result = false;  //nếu không thành công thì trả về false
        }
        else
        {   
            result = true;
        }
    }
    return result;
}
bool checkInFileTable(char  temp[100])  //kiểm tra xem fileDescriptor có tồn tại trong filedescriptorTable không ?
{
    SysPrintString(temp);
    for (int i = 2; i < FILE_MAX; i++)
    {
        if (fileDescriptorTable[i] == NULL)
        {

            return false;
        }
        else{
             
        }
        if (strcmp(fileDescriptorTable[i]->name, temp)==0)
        {

            return true;
        }
    }
    return false;
}
bool SysRemoveFile(char *nameFile)
{
    bool result;
    int length = strlen(nameFile);

    int check = checkInFileTable(nameFile); //kiểm tra xem file có tồn tại trong mảng fileTable không 
    if (check == false) //nếu không tồn tại thì thực hiện xóa file
    {
        if (length == 0)
        {
            DEBUG(dbgSys, "\nFile name can't be empty");
            result = false;
        }
        else if (nameFile == NULL)
        {
            DEBUG(dbgSys, "\nNot enough memory in system");
            result = false;
        }
        else
        {
            if (!kernel->fileSystem->Remove(nameFile)) //gọi remove thông qua lớp fileSystem
            {
                DEBUG(dbgSys, "\nXoa that bai");
                result = false;
            }
            else
            {
                DEBUG(dbgSys, "\nXoa thanh cong");
                result = true;
            }
        }
        return result;
    }
    else
    {
        return false;
    }
}

int SysSeek(int posSeek, int fileID)
{
    if (fileID <= 1)
    {
        return -1;
    }

   // kiểm tra fileId có hợp lệ hay không
    if (fileID <= 1 || fileID >= FILE_MAX)
        return -1;
    if (fileDescriptorTable[fileID] == NULL)
        return -1;

    if (posSeek == -1)
        posSeek = fileDescriptorTable[fileID]->openFile->Length(); //gọi hàm length thông qua lớp openfile để lấy độ dài file

    if (posSeek < 0 || posSeek > fileDescriptorTable[fileID]->openFile->Length()) //kiểm tra nếu vị trí seek vượt ra khỏi độ dài của file
        return -1;

    fileDescriptorTable[fileID]->openFile->Seek(posSeek); // thực hiện cập nhật lại con trỏ trong file thông qua thủ tục seek của lớp openFile

    return posSeek;
}
/*read and write file*/
/*read and write file*/
int GetString(char *buffer, int size) //
{
    for (int i = 0; i < size; ++i)
    {
        buffer[i] = kernel->synchConsoleIn->GetChar(); // đọc ký tự và lưu dưới dạng char (read console )
        if (buffer[i] == EOF)
        {
            buffer[i] = 0;//đọc hết file trả về -2
            return -2;
        }
    }

    return size;
}
int Read(char *buffer, int charCount, int index) // 
{
    if (index >= FILE_MAX) //index vượt quá số chỉ file_max đã định, trả về -1
        return -1;
    if (fileDescriptorTable[index] == NULL)//nếu không tồn tại filetable trả về -1
        return -1;
    int result = fileDescriptorTable[index]->openFile->Read(buffer, charCount);//đọc số các bytes từ file theo vị trí tăng dần trong file
    if (result != charCount)
        return -2; //trả về 2 nếu result khác charCount
    return result;
}


int SysRead(char *buffer, int charCount, int fileId)
{
    if (fileId == 0)
    {
        return GetString(buffer, charCount); //nếu ID=0, đọc vào console
    }
    return Read(buffer, charCount, fileId);// đọc file 
}

int PutString(char *buffer, int size)
{
    for (int i = 0; i < size; ++i)
        kernel->synchConsoleOut->PutChar(buffer[i]);
        //ghi lần lượt từng ký tự lên màn hình console
    return size;
}

int Write(char *buffer, int charCount, int index)
{
    if (index >= FILE_MAX)//index vượt quá số chỉ file_max đã định, trả về -1
        return -1;
    if (fileDescriptorTable[index] == NULL || fileOpenMode[index] == MODE_READ)//nếu không tồn tại filetable hoặc mode là read (không có write) trả về -1
        return -1;
    return fileDescriptorTable[index]->openFile->Write(buffer, charCount);
    //ghi các bytes từ file theo vị trí tăng dần trong file
}

int SysWrite(char *buffer, int charCount, int fileId)
{
    int realLength = strlen(buffer);//kích thước thật sự của file
    if (fileId == 1)
    {
        return PutString(buffer, realLength);
        //nếu ID=1, ghi buffer lên console display
    }
    return Write(buffer, realLength, fileId);// ghi file
}



/*open file and close file*/
int open_file(char *fileName, int openMode)
{
    int freeIndex = -1;
    int fileDescriptor = -1;
    // return 1;
    //kiểm tra còn chỗ trống trong mảng fileDescriptorTable
    for (int i = 2; i < FILE_MAX; i++)
    {

        if (fileDescriptorTable[i] == NULL)
        {

            freeIndex = i;
            break;
        }
    }
    if (freeIndex == -1)
    {
        return -1;
    }
    //kiểm tra openmode và chọn kiểu open tương ứng
    if (openMode == MODE_READWRITE)
        fileDescriptor = OpenForReadWrite(fileName, FALSE);
    if (openMode == MODE_READ)
        fileDescriptor = OpenForRead(fileName, FALSE);

    if (fileDescriptor == -1)
        return -1;
    fileDescriptorTable[freeIndex]= new file(fileName,openMode);
    fileDescriptorTable[freeIndex]->openFile=kernel->fileSystem->Open(fileName);
    
    //trả về id của openfile
    return freeIndex;
}

int SysOpen(char *fileName, int type)
{

//nếu type khác 0 và 1 thì return -1
    if (type != 0 && type != 1)
        return -1;
    int id = open_file(fileName, type);
    if (id == -1)
        return -1;
    DEBUG(dbgSys, "\nOpened file");
    //trả về id
    return id;
}
int close(int index)
{
    //nếu id không hợp lệ thì return -1
    if (index < 2 || index >= FILE_MAX)
        return -1;
    //kiểm tra xem tồn tại trong mảng fileDescriptorTable hay không ?
    if ( fileDescriptorTable[index])
    {

        //thực hiện xóa phần tử trong mảng
        delete fileDescriptorTable[index];
        fileDescriptorTable[index]=NULL;
        return 0;
    }
    return -1;
}

int SysClose(int id)
{
    return close(id);
}
#endif /* ! __USERPROG_KSYSCALL_H__ */
