// The 'server.c' code goes here.

#include<stdio.h>
#include<unistd.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Server Domain'.

int main(int argc, char *argv[])
{

	printf("I am the server.\n");
	printf("Server IP address: %s\n", argv[1]);
	md5_print();
	printf("-----------\n");
	
	return 0;
}
