// The 'client.c' code goes here.
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Client Domain'.
#define PORT 9999
void upload(char file) {
     int err,n;
     unsigned char buffer[4096];
     // FILE *file;
     //if file exists, open file and read file
     if (file = fopen(file, "r")) {
         //TODO: open remote directory??
         //write file from local to remote
         while (1) {
             err = read(file, buffer, 4096);
             //file is not found
             if (err = -1) {
                 //print error message
                 print("File %s could not be uploaded successfully", file);
             }
             //write file to the remote directory
             err = write("Remote Directory", buffer, n);
         }

     }
 }
int download(int client_socket, char destination_path[]){
    int received_size;
    // char destination_path[] = "Local Directory/client_file.txt";  // Note how we don't have the original file name.
    int chunk_size = 1000;
    char file_chunk[chunk_size];
//    int chunk_counter = 0;

    FILE *fptr;

    // Opening a new file in write-binary mode to write the received file bytes into the disk using fptr.
    fptr = fopen(destination_path,"wb");

    // Keep receiving bytes until we receive the whole file.
    while (1){
        bzero(file_chunk, chunk_size);
//        memset(&file_chunk, 0, chunk_size);

        // Receiving bytes from the socket.
        received_size = recv(client_socket, file_chunk, chunk_size, 0);
        printf("Client: received %i bytes from server.\n", received_size);

        // The server has closed the connection.
        // Note: the server will only close the connection when the application terminates.
        if (received_size == 0){
            close(client_socket);
            fclose(fptr);
            break;
        }
        // Writing the received bytes into disk.
        fwrite(&file_chunk, sizeof(char), received_size, fptr);
//        printf("Client: file_chunk data is:\n%s\n\n", file_chunk);
    }
}
void readFile(FILE *input_file) {
     FILE *input_file = fopen("user_commands.txt", "r");
     // char line[500];
     char line = "download";
     //reads the file line by line
    //  while (fgets(line, sizeof(line), input_file)) {
         //extracts first token in line (i.e. the command)
        char *token = strtok(line, " ");
        printf("%s", token); 
         //send different message for each command 
        //  if (token == "append") {
        //      //send and receive an entire file
        //      //send_append(client_socket, file)
        //  }
        //  else if (token == "upload") {
        //      //send_upload(client_socket, file)
        //  }
        //  else if (token == "download") {
        //      //send_download(client_socket, file)
        //     printf("download\n"); 
        //  }
        //  else if (token == "delete") {
        //      //send_delete(client_socket, file)
        //  }
        //  else if (token == "syncheck") {
        //      //send_append(client_socket, file)
        //  }
        //  else if (token == "quit") {
        //      //quit
        //  }
     // }
 }
int start_client(char const* const fileName, char ipAddress[])
{
    int client_socket;
    struct sockaddr_in serv_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (client_socket < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // The server IP address should be supplied as an argument when running the application.
    int addr_status = inet_pton(AF_INET, ipAddress, &serv_addr.sin_addr);
    if (addr_status <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    int connect_status = connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (connect_status < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    ///////////// Start sending and receiving process //////////////
    char message[] = "download server_file.txt"; 
    send(client_socket, message, strlen(message), 0);
    download(client_socket, "Local Directory/client_file.txt");
    close(client_socket);
    FILE *input_file = fopen(fileName, "r");
    readFile(input_file);
    


}

int main(int argc, char *argv[])
{

	printf("I am the client.\n");
	printf("Input file name: %s\n", argv[1]);
	printf("My server IP address: %s\n", argv[2]);
	md5_print();
	printf("-----------\n");
    char const* const fileName = "user_command.txt";
	start_client(fileName, argv[2]); 
	exit(0);
	return 0;
}
