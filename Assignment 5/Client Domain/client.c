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
        send(client_socket, "valid", 5, 0); 
        sleep(1);   
        // printf("here\n"); 
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
        // close(client_socket);
        // close(server_socket);
        fclose(fptr);
        printf("%d bytes uploaded successfully.\n", total_bytes); 
        }
        else{
            send (client_socket, "invalid", 7, 0); 
            printf("File [%s] could not be found in local directory\n", fileName); 
        }
    return 0;
 }
int syncheck_download(int client_socket, char destination_path[], char fileName[]){
    int received_size;
    // char destination_path[] = "Local Directory/client_file.txt";  // Note how we don't have the original file name.
    int chunk_size = 1000;
    char file_chunk[chunk_size];
    int chunk_counter = 0;
    char error[8];
    recv(client_socket, error, 10, 0); 
    // printf("%s, %d", error), strncmp(error, "valid", 5); 
    // Opening a new file in write-binary mode to write the received file bytes into the disk using fptr.
    if (strncmp(error, "valid", 5) == 0){    
        FILE *fptr;
        fptr = fopen(destination_path,"wb");

        // Keep receiving bytes until we receive the whole file.
        while (1){
            bzero(file_chunk, chunk_size);
    //        memset(&file_chunk, 0, chunk_size);

            // Receiving bytes from the socket.
            received_size = recv(client_socket, file_chunk, chunk_size, 0);
            chunk_counter += received_size; 
            // printf("Client: received %i bytes from server.\n", received_size);

            // The server has closed the connection.
            // Note: the server will only close the connection when the application terminates.
            if (received_size == 0){
                fclose(fptr);
                break;
            }
            // Writing the received bytes into disk.
            fwrite(&file_chunk, sizeof(char), received_size, fptr);
    //        printf("Client: file_chunk data is:\n%s\n\n", file_chunk);
        }
    }
    // printf("%d\n", chunk_counter);
}
int download(int client_socket, char destination_path[], char fileName[]){
    int received_size;
    // char destination_path[] = "Local Directory/client_file.txt";  // Note how we don't have the original file name.
    int chunk_size = 1000;
    char file_chunk[chunk_size];
    int chunk_counter = 0;
    char error[8];
    recv(client_socket, error, 10, 0); 
    // printf("%s, %d", error), strncmp(error, "valid", 5); 
    // Opening a new file in write-binary mode to write the received file bytes into the disk using fptr.
    if (strncmp(error, "valid", 5) == 0){    
        FILE *fptr;
        fptr = fopen(destination_path,"wb");

        // Keep receiving bytes until we receive the whole file.
        while (1){
            bzero(file_chunk, chunk_size);
    //        memset(&file_chunk, 0, chunk_size);

            // Receiving bytes from the socket.
            received_size = recv(client_socket, file_chunk, chunk_size, 0);
            chunk_counter += received_size; 
            // printf("Client: received %i bytes from server.\n", received_size);

            // The server has closed the connection.
            // Note: the server will only close the connection when the application terminates.
            if (received_size == 0){
                printf("%d bytes downloaded successfully.\n",chunk_counter);
                fclose(fptr);
                break;
            }
            // Writing the received bytes into disk.
            fwrite(&file_chunk, sizeof(char), received_size, fptr);
    //        printf("Client: file_chunk data is:\n%s\n\n", file_chunk);
        }
    }
    else{
        printf("File [%s] could not be found in remote directory.\n", fileName); 
    }
    // printf("%d\n", chunk_counter);
}

//  the user can type “delete <file_name>” command to delete <file_name> on the remote
// directory. If the file doesn’t exist in the remote directory on the server, the application prints “File
// <file_name> could not be found in remote directory.”. Otherwise, the server deletes the file from
// the remote directory and the client terminal prints a success message. 
int delete(int client_socket, char fileName[]){
    char error [10];
    recv(client_socket, error, 10, 0); 
    if (strncmp("valid", error, 5) == 0){
        printf("File deleted successfully.\n"); 
    }
    else{
        printf("File %s could not be found in remote directory.\n", fileName);
    }

}
int append (int client_socket, FILE *fptr, char fileName[]){
    char line[500]; 
    recv(client_socket, line, sizeof(line), 0);    if (strncmp(line, "valid", 5) == 0){
        while (1){
            fgets(line, sizeof(line), fptr);   
            send(client_socket, line, sizeof(line),0);
            if (strncmp(line, "pause", 5) == 0){
                char *token;
                token = strtok(line, " ");
                token = strtok(NULL, " ");
                printf("%s", token); 
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
int compare(FILE *fptr1, FILE *fptr2){
    char * line1 = NULL;
    char * line2 = NULL;
    size_t len = 0;

    if (fptr1 == NULL || fptr2 == NULL){
        fclose(fptr1);
        fclose(fptr2);
        return 1; 
    }

    while ((getline(&line1, &len, fptr1)) != -1 && (getline(&line2, &len, fptr2)) != -1) {
        if (strcmp(line1, line2) != 0){
            return 1;
        }
    }

    fclose(fptr1);
    fclose(fptr2);
    return 0; 
}
int syncheck(int client_socket, char fileName[]){
    FILE *fptr;
    int file_size; 
    char path[100] = "Local Directory/"; 
    int inLocal = 0; 
    char inRemote[100]; 
    strcat(path, fileName); 
    printf("Sync Check Report:\n");
    // printf("%s\n",path); 
    fptr = fopen(path,"rb"); 
    if (fptr){
        inLocal = 1; 
        printf("- Local Directory:\n");
        fseek(fptr, 0L, SEEK_END);  // Sets the pointer at the end of the file.
        file_size = ftell(fptr);  // Get file size.
        fseek(fptr, 0L, SEEK_SET);
        printf("-- File Size: %d bytes.\n", file_size);
    }
    send(client_socket, "ready", 5, 0); 
    recv(client_socket, inRemote, sizeof(inRemote), 0); 
    if (strncmp(inRemote, "0",1) != 0){
        printf("- Remote Directory: \n-- File Size: %s bytes.\n", inRemote); 
        syncheck_download(client_socket, "Local Directory/temp.txt", fileName); 
        if (inLocal == 1 && compare(fopen("Local Directory/temp.txt", "rb"), fopen(path, "rb")) == 0){
            printf("-- Sync Status: synced.\n");
        }
        else{
            printf("-- Sync Status: unsynced.\n");
        }
        remove("Local Directory/temp.txt") == 0;
    }

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
    // char message[] = "download server_file.txt"; 
    // send(client_socket, message, strlen(message), 0);
    // download(client_socket, "Local Directory/abx.txt", "abx.txt");
    //upload(client_socket, "Local Directory/client_file.txt", "client_file.txt");
    // delete(client_socket, "client_file.txt"); 
    // append(client_socket, fopen("user_command.txt", "rb"), "server_file.txt"); 
    syncheck(client_socket, "server_file.txt");
    compare(fopen("Local Directory/client_file.txt", "rb"), fopen("Local Directory/server_file.txt", "rb"));
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
    char const* const fileName = "user_command.txt";
	start_client(fileName, argv[2]); 
	exit(0);
	return 0;
}