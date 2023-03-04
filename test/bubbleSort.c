#include "syscall.h"

int main()
{
	int arr[100];
	int i, j, temp;
	int n;
	int	choice;
	PrintString("Enter number of array (n < 100) : ");
	n = ReadNum();
	PrintChar('\n');
	PrintString("Enter array elements : \n");

	for (i = 0; i < n; i++)
	{
		arr[i] = ReadNum();
	}
	PrintString("Enter your choice ( 0: ascending -> ; 1: descending <- ) : \n");
	PrintString("choice ! : \n");
	choice=ReadNum();
	if (choice == 0)
	{
		// Burble sort ascending
		for (i = 0; i < n - 1; i++)
		{
			for (j = n - 1; j > i; j--)
			{
				if (arr[j] < arr[j - 1])
				{
					temp = arr[j];
					arr[j] = arr[j - 1];
					arr[j - 1] = temp;
				}
			}
		}
	}

	else if (choice == 1)
	{
		// Burble sort descending
		for (i = 0; i < n - 1; i++)
		{
			for (j = n - 1; j > i; j--)
			{
				if (arr[j] > arr[j - 1])
				{
					temp = arr[j];
					arr[j] = arr[j - 1];
					arr[j - 1] = temp;
				}
			}
		}
	}

	else
	{
		PrintString("Wrong choice ! \n");
		return 0;
	}

	PrintString("Sorted array: ");
	for (i = 0; i < n; i++)
	{
		PrintNum(arr[i]);
		PrintChar(' ');
	}
	Halt();
}
