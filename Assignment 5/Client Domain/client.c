// The 'client.c' code goes here.
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Client Domain'.
#define PORT 9999
int upload(int client_socket, char source_path[], char fileName[]) {
    FILE *fptr;
    int chunk_size = 1000;
    char file_chunk[chunk_size];
    fptr = fopen(source_path,"rb"); 
    if (fptr){
        send(client_socket, "valid", 5, 0);         // printf("here\n"); 
         // Open a file in read-binary mode.
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
            fclose(fptr);
            printf("%d bytes uploaded successfully.\n", total_bytes); 
        }
        else{
            send (client_socket, "invalid", 7, 0); 
            printf("File [%s] could not be found in local directory\n", fileName); 
        }
    return 0;
 }
int append (int client_socket, FILE *fptr, char fileName[]){
    char line[500]; 
    recv(client_socket, line, sizeof(line), 0);    
    if (strncmp(line, "valid", 5) == 0){
        while (1){
            fgets(line, sizeof(line), fptr);   
            send(client_socket, line, sizeof(line),0);
            sleep(1); 
            if (strncmp(line, "pause", 5) == 0){
                char *token;
                token = strtok(line, " ");
                token = strtok(NULL, " ");
                // printf("%s", token); 
                sleep(atoi(token)); 
            }
            else if (strncmp(line, "close", 5) == 0){
                break;
            }
            else{
                
                printf("Appending> %s", line); 
            }
        }
    }
    else {
        printf("File [%s] could not be found in remote directory.\n", fileName); 
    }
}

void readFile(char filename[], int client_socket) {
    FILE *input_file = fopen(filename, "r");
    char line[500];
    char buffer [5]; 
    pthread_mutex_t mutexLock = pthread_mutex_init(&lock, NULL);
    while (fgets(line, sizeof(line), input_file)) {
        char *token;
        char *fileName; 
        char path[100] = "Local Directory/";
        send(client_socket, line, sizeof(line), 0); 
        recv(client_socket, buffer, sizeof(buffer),0); 
        // printf("s %s", line);
        token = strtok(line, " ");
        if (strcmp("quit", token) == 0){
            //close(client_socket);
            break;
        }
        
        fileName = strtok(NULL, "\n");
        strcat(path, fileName);
        //TODO: Check to make sure the file is locked or unlocked
        //Mark a file as locked or unlocked 
        //if file is unlocked: run these if statements
        if (buffer == "unlocked") { //status if unlocked
            if (strcmp("append", token) == 0){
                append(client_socket, input_file, fileName); 
                //when close command is issued
                //then unlock the file
            }
            if (strcmp("upload", token) == 0){
                // Release the lock for the next thread.
                // pthread_mutex_unlock(&lock);
                upload(client_socket, path, fileName); 
            }
        else {
            printf("File %s is currently locked by another user.", fileName);
        }

    
    }
 }


int start_client(char fileName[], char ipAddress[])
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
    // char message[] = "download server_file.txt"; 
    // send(client_socket, message, strlen(message), 0);
    // download(client_socket, "Local Directory/server_file.txt", "server_file.txt");
    // upload(client_socket, "Local Directory/client_file.txt", "client_file.txt");
    // delete(client_socket, "client_file.txt"); 
    // append(client_socket, fopen("user_command.txt", "rb"), "server_file.txt"); 
    // syncheck(client_socket, "server_file.txt");
    // compare(fopen("Local Directory/client_file.txt", "rb"), fopen("Local Directory/server_file.txt", "rb"));    readFile("user_command.txt",client_socket);
    readFile(fileName, client_socket); 
    close(client_socket);
    
    return 0; 
}


int main(int argc, char *argv[])
{

	// printf("I am the client.\n");
	// printf("Input file name: %s\n", argv[1]);
	// printf("My server IP address: %s\n", argv[2]);
	// md5_print();
	// printf("-----------\n");
    printf("Welcome to ICS53 Online Cloud Storage.\n"); 
	start_client(argv[1], argv[2]); 
	exit(0);
	return 0;
}