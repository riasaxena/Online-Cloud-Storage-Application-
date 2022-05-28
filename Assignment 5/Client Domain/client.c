// The 'client.c' code goes here.

#include<stdio.h>
#include<unistd.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Client Domain'.

int main(int argc, char *argv[])
{

	printf("I am the client.\n");
	printf("Input file name: %s\n", argv[1]);
	printf("My server IP address: %s\n", argv[2]);
	md5_print();
	printf("-----------\n");
	
	return 0;
}
