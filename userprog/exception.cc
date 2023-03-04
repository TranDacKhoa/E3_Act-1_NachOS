// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void increasePC()
{
	/* set previous programm counter (debugging only)
	 * similar to: registers[PrevPCReg] = registers[PCReg];*/
	kernel->machine->WriteRegister(PrevPCReg,
								   kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction
	 * similar to: registers[PCReg] = registers[NextPCReg]*/
	kernel->machine->WriteRegister(PCReg,
								   kernel->machine->ReadRegister(NextPCReg));

	/* set next programm counter for brach execution
	 * similar to: registers[NextPCReg] = pcAfter;*/
	kernel->machine->WriteRegister(
		NextPCReg, kernel->machine->ReadRegister(NextPCReg) + 4);
}

void do_ReadNum()
{
	int result = SysReadNum();
	kernel->machine->WriteRegister(2, result);
	return increasePC();
}
void do_PrintNum()
{
	int character = kernel->machine->ReadRegister(4);
	SysPrintNum(character);
	return increasePC();
}
void do_RandomNum()
{
	int res = SysRandom();
	kernel->machine->WriteRegister(2, res);
	return increasePC();
}

void stringSystem2User(char *buffer, int addr, int convert_length = -1)
{
	int length = (convert_length == -1 ? strlen(buffer) : convert_length);
	for (int i = 0; i < length; i++)
	{
		kernel->machine->WriteMem(addr + i, 1, buffer[i]); // sao chép từ  kí tự vào  user space
	}
	kernel->machine->WriteMem(addr + length, 1, '\0');
}
char *stringUser2System(int memPtr,int convert_length = -1)
{
	int length = 0;
	bool flag = false;

	char *buffer;
	// vòng lặp để đếm số lượng kí tự của chuỗi
	while (flag != true)
	{
		int character;
		kernel->machine->ReadMem(memPtr + length, 1, &character); // sao chép kí tự vào kernel space
		length++;

		if ((character == '\0'  && convert_length == -1)||
                length == convert_length)
		{
			flag = true;
		}
	}
	buffer = new char[length];

	for (int i = 0; i < length; i++)

	{
		int character;
		kernel->machine->ReadMem(memPtr + i, 1, &character);
		buffer[i] = (unsigned char)character;
	}
	return buffer;
}
void do_ReadChar()
{
	DEBUG(dbgSys, "Nhap ki tu: ");
	char character;
	character = SysReadChar();
	kernel->machine->WriteRegister(2, (int)character); // ghi ki tu duoi dang ma ASCII len thanh ghi 2

	return increasePC();

	// ASSERTNOTREACHED();
}
void do_PrintChar()
{
	DEBUG(dbgSys, "In ky tu: ");
	char c;
	c = (char)kernel->machine->ReadRegister(4); // đọc mã ascii từ thanh ghi số 4 và ép kiểu về char
	SysPrintChar(c);

	return increasePC();
	// ASSERTNOTREACHED();
}
void do_ReadString()
{
	DEBUG(dbgSys, "Nhap chuoi: ");
	int memoryPointer;
	memoryPointer = kernel->machine->ReadRegister(4); // lấy địa chỉ từ thanh ghi số 4
	int length;
	length = kernel->machine->ReadRegister(5); // lấy độ dài của chuỗi từ thanh ghi 5

	char *buffer;
	buffer = SysReadString(length); // nhap chuoi
	if (buffer != NULL)
	{

		stringSystem2User(buffer, memoryPointer); // chuyển đổi vùng nhớ từ Kernel space sang User space
	}
	else
	{
		DEBUG(dbgSys, "Khong nhap dung chuoi ");
	}

	delete[] buffer;
	return increasePC();

	ASSERTNOTREACHED();
}
void do_PrintString()
{
	// DEBUG(dbgSys, "In chuoi: ");
	int memPtr;
	memPtr = kernel->machine->ReadRegister(4);
	char *string;
	string = stringUser2System(memPtr); // chuyển đổi vùng nhớ từ User space sang  Kernel space
	SysPrintString(string);
	delete[] string;

	return increasePC();

	ASSERTNOTREACHED();
}

/* do an 2 section */
/*create, delete and seek*/
void do_CreateFile()
{
	int memPtr;
	memPtr = kernel->machine->ReadRegister(4);

	char *nameFile;
	nameFile = stringUser2System(memPtr);

	if (SysCreateFile(nameFile) == true)
	{
		kernel->machine->WriteRegister(2, 0); // trả về 0 nếu thành công
	}
	else
	{
		kernel->machine->WriteRegister(2, -1); // trả về -1 khi thất bại
	}
	delete[] nameFile;

	return increasePC();

	// ASSERTNOTREACHED();
}
void do_RemoveFile()
{
	int memPtr;
	memPtr = kernel->machine->ReadRegister(4);

	char *nameFile;
	nameFile = stringUser2System(memPtr);

	if (SysRemoveFile(nameFile) == true)
	{
		kernel->machine->WriteRegister(2, 0);
	}
	else
	{
		kernel->machine->WriteRegister(2, -1);
	}
	delete[] nameFile;
	return increasePC();
}
void do_Seek()
{
	int seekPos = kernel->machine->ReadRegister(4);
	int fileID = kernel->machine->ReadRegister(5);

	int result;
	result = SysSeek(seekPos, fileID);
	kernel->machine->WriteRegister(2, result);
	return increasePC();
}

/*open and close file*/
void do_OpenFile()
{
	int virtAddr = kernel->machine->ReadRegister(4);
	char *fileName = stringUser2System(virtAddr);
	int type = kernel->machine->ReadRegister(5);

	kernel->machine->WriteRegister(2, SysOpen(fileName, type));

	delete fileName;
	return increasePC();
}
void do_CloseFile()
{
	int id = kernel->machine->ReadRegister(4);
	kernel->machine->WriteRegister(2, SysClose(id));

	return increasePC();
}



void do_Read() {
    int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char* buffer = stringUser2System(virtAddr, charCount);
    int fileId = kernel->machine->ReadRegister(6);

    DEBUG(dbgFile,
          "Read " << charCount << " chars from file " << fileId << "\n");

    kernel->machine->WriteRegister(2, SysRead(buffer, charCount, fileId));
    stringSystem2User(buffer, virtAddr, charCount);

    delete[] buffer;
    return increasePC();
}

void do_Write() {
    int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char* buffer = stringUser2System(virtAddr, charCount);
    int fileId = kernel->machine->ReadRegister(6);

    DEBUG(dbgFile,
          "Write " << charCount << " chars to file " << fileId << "\n");

    kernel->machine->WriteRegister(2, SysWrite(buffer, charCount, fileId));
     stringSystem2User(buffer, virtAddr, charCount);

    delete[] buffer;
    return increasePC();
}
void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
	// 1) Minh da viet lai nhung cai exception, voi nhung cai bi loi thi no se thong bao loi va goi syshalt()
	switch (which)
	{
	case NoException:
		return;

	case PageFaultException:
		DEBUG('a', "\n No valid translation found");
		printf("\n\n No valid translation found");
		SysHalt();

		break;

	case ReadOnlyException:
		DEBUG('a', "\n Write attemped to page marked read-only");
		printf("\n\n Write attemped to page marked read-only");
		SysHalt();

		break;

	case BusErrorException:
		DEBUG('a', "\n Translation resulted invalid physical address");
		printf("\n\n Translation resulted invalid physical address");
		SysHalt();

		break;

	case AddressErrorException:
		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		SysHalt();

		break;

	case OverflowException:
		DEBUG('a', "\n Integer overflow in add or sub");
		printf("\n\n Integer overflow in add or sub");
		SysHalt();

		break;

	case IllegalInstrException:
		DEBUG('a', "\n Unimplemented or reserved instr");
		printf("\n\n Unimplemented or reserved instr");
		SysHalt();

		break;

	case NumExceptionTypes:
		DEBUG('a', "\n Number exception types");
		printf("\n\n Number exception types");
		SysHalt();

		break;

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			// ASSERTNOTREACHED();
		case SC_ReadNum:
			return do_ReadNum();
			break;
		case SC_PrintNum:
			return do_PrintNum();
			break;
		case SC_Random:
			return do_RandomNum();
			break;
		case SC_ReadChar:
			return do_ReadChar();
			break;
		case SC_PrintChar:
			return do_PrintChar();
			break;
		case SC_ReadString:
			return do_ReadString();
			break;
		case SC_PrintString:
			return do_PrintString();
			break;
		case SC_Open:
			return do_OpenFile();
			break;
		case SC_Close:
			return do_CloseFile();
			break;
		case SC_CreateFile:
			return do_CreateFile();
			break;
		case SC_RemoveFile:
			return do_RemoveFile();
			break;
		case SC_Seek:
			return do_Seek();
			break;
		case SC_Read:
			return do_Read();
			break;
		case SC_Write:
			return do_Write();
			break;
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
