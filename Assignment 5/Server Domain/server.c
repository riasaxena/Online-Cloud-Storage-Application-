// The 'server.c' code goes here.
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Server Domain'.
#define PORT 9999
int upload(int client_socket, int server_socket, char destination_path[]){
    send(client_socket, "ready", 5, 0); 
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
        // printf("Client: received %i bytes from server.\n", received_size);

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

int download(int client_socket, int server_socket, char source_path[]){
    FILE *fptr;
    int chunk_size = 1000;
    char file_chunk[chunk_size];
    // char source_path[] = "Remote Directory/server_file.txt";
    fptr = fopen(source_path,"rb");  // Open a file in read-binary mode.
    fseek(fptr, 0L, SEEK_END);  // Sets the pointer at the end of the file.
    int file_size = ftell(fptr);  // Get file size.
    // printf("Server: file size = %i bytes\n", file_size);
    fseek(fptr, 0L, SEEK_SET);  // Sets the pointer back to the beginning of the file.

    int total_bytes = 0;  // Keep track of how many bytes we read so far.
    int current_chunk_size;  // Keep track of how many bytes we were able to read from file (helpful for the last chunk).
    ssize_t sent_bytes;

    while (total_bytes < file_size){
        // Clean the memory of previous bytes.
        // Both 'bzero' and 'memset' works fine.
        bzero(file_chunk, chunk_size);
//        memset(file_chunk, '\0', chunk_size);

        // Read file bytes from file.
        current_chunk_size = fread(&file_chunk, sizeof(char), chunk_size, fptr);

        // Sending a chunk of file to the socket.
        sent_bytes = send(client_socket, &file_chunk, current_chunk_size, 0);

        // Keep track of how many bytes we read/sent so far.
//        total_bytes = total_bytes + current_chunk_size;
        total_bytes = total_bytes + sent_bytes;

        // printf("Server: sent to client %i bytes. Total bytes sent so far = %i.\n", sent_bytes, total_bytes);

    }
    // close(client_socket);
    // close(server_socket);
    fclose(fptr);
    return 0;
}

int start_server()
{
    int client_socket, server_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);


    // Creating socket file descriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int socket_status = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt));
    if (socket_status) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
 	address.sin_addr.s_addr = INADDR_ANY;
    // The server IP address should be supplied as an argument when running the application.
    // address.sin_addr.s_addr = inet_addr(ipAddress);
    address.sin_port = htons(PORT);

    int bind_status = bind(server_socket, (struct sockaddr*)&address, sizeof(address));
    if (bind_status < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    int listen_status = listen(server_socket, 1);
    if (listen_status < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    client_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }


    ///////////// Start sending and receiving process //////////////
    char buffer[1024];
    recv(client_socket, buffer, 1024, 0);
    // printf("%s\n", buffer); 
    char *token;
    token = strtok(buffer, " ");
    if (strcmp(token, "download") == 0){
        token = strtok (NULL, " ");
        char path[100];
        strcpy(path, "Remote Directory/"); 
        strcat(path, token);
        download(client_socket, server_socket, path); 
    }
    else if (strcmp(token, "upload") == 0){
        token = strtok (NULL, " ");
        char path[100];
        strcpy(path, "Remote Directory/"); 
        strcat(path, token);
        upload(client_socket, server_socket, path); 
    }
    
    close(client_socket);
    close(server_socket);
    return 0;

}


int main(int argc, char *argv[])
{

	printf("I am the server.\n");
	printf("Server IP address: %s\n", argv[1]);
	md5_print();
	printf("-----------\n");
	start_server(); 
	exit(0);
	return 0;
}