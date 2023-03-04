#include "syscall.h"
int main()
{
	char i = 32;
	for (; i < 127; i++)
	{
		PrintNum((int)i);
		PrintChar(':');
		PrintChar(i);
		PrintChar('\n');
	}
	Halt();
}
