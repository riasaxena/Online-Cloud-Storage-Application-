// The 'server.c' code goes here.
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Server Domain'.
#define PORT 9999
pthread_t threads[5];
// pthread_t thread;
int global_variable = 5;
pthread_mutex_t lock;
int server_socket;
void INThandler(int);

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     close(server_socket);
     exit(0); 
}

int upload(int client_socket, char destination_path[]){
    int received_size;
    // char destination_path[] = "Local Directory/client_file.txt";  // Note how we don't have the original file name.
    int chunk_size = 1000;
    char file_chunk[chunk_size];
    // int chunk_counter = 0;
    char error[8];
    recv(client_socket, error, 10, 0); 
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
            // chunk_counter += received_size; 
            // printf("Client: received %i bytes from server.\n", received_size);

            // The server has closed the connection.
            // Note: the server will only close the connection when the application terminates.
            if (received_size == 0){
                // printf("%d bytes uploaded successfully.\n",chunk_counter);
                fclose(fptr);
                break;
            }
            // Writing the received bytes into disk.
            fwrite(&file_chunk, sizeof(char), received_size, fptr);
    //        printf("Client: file_chunk data is:\n%s\n\n", file_chunk);
        }
    }
    return 0; 
}

int download(int client_socket,  char source_path[]){
    FILE *fptr;
    int chunk_size = 1000;
    char file_chunk[chunk_size];
    fptr = fopen(source_path,"rb"); 
    char checker[8];

    if (fptr){
        send(client_socket, "valid", 5, 0); 
        recv(client_socket, checker, sizeof(checker), 0); 
         // Open a file in read-binary mode.
        fseek(fptr, 0L, SEEK_END);  // Sets the pointer at the end of the file.
        int file_size = ftell(fptr);  // Get file size.
        // printf("Server: file size = %i bytes\n", file_size);
        fseek(fptr, 0L, SEEK_SET);  // Sets the pointer back to the beginning of the file.

        int total_bytes = 0;  // Keep track of how many bytes we read so far.
        int current_chunk_size;  // Keep track of how many bytes we were able to read from file (helpful for the last chunk).
        ssize_t sent_bytes;

        while (total_bytes < file_size){
            printf("here"); 
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

            printf("Server: sent to client %i bytes. Total bytes sent so far = %i.\n", sent_bytes, total_bytes);
            recv(client_socket, checker, sizeof(checker), 0);
            printf("%s\n", checker);
        }
        // close(client_socket);
        // close(server_socket);
        fclose(fptr);
        }
        else{
            send (client_socket, "invalid", 7, 0); 
        }
    return 0;
}

int delete(int client_socket, char destination_path[]){
    FILE *file;
    //open and only read the folder 
        if (file = fopen(destination_path, "r")) {
            //remove file from the directory
            remove(destination_path) == 0;
            send(client_socket,"valid", 5, 0);
            fclose(file);
        }
        else {
            send(client_socket, "invalid", 7, 0); 
            // printf("File %s could not be found in remote directory.", destination_path);
        } 
}
int append(int client_socket, char destination_path[]){
    FILE *fptr;
    char line [500]; 
    fptr = fopen(destination_path,"a");
    int received_size;
    if (fptr){  
        send(client_socket, "valid", 5, 0); 
        sleep (1);  
        // fseek(fptr, 0L, SEEK_END); 
        fwrite("\n", sizeof(char), strlen("\n"), fptr);
        while (1){    
            recv(client_socket, line, sizeof(line), 0);
            if (strncmp(line, "pause", 5) == 0){
                char *token;
                token = strtok(line, " ");
                token = strtok(NULL, " ");
                sleep(atoi(token)); 
            } 
            else if (strncmp(line, "close", 5) == 0){
                break;
            }
            else {
                fwrite(line, sizeof(char), strlen(line), fptr);
            }
        } 
    }
    else{
        send(client_socket, "invalid", 7, 0); 
    }

}
int syncheck(int client_socket, char fileName[]){
    FILE *fptr;
    char buffer [100];
    int file_size = 0; 
    recv(client_socket, buffer, 100, 0); 
    char path[100] = "Remote Directory/"; 
    strcat(path, fileName); 
    // printf("%s\n",path); 
    fptr = fopen(path,"rb"); 
    if (fptr){
        // printf("here\n"); 
        fseek(fptr, 0L, SEEK_END);  // Sets the pointer at the end of the file.
        file_size = ftell(fptr);  // Get file size.
        // printf("%d\n", file_size); 
        fseek(fptr, 0L, SEEK_SET);
        
    }
    // printf("%d\n", file_size);
    sprintf(buffer, "%d", file_size);
    send(client_socket, buffer, sizeof(buffer), 0);
    if (file_size > 0) {
        download(client_socket,  path);
    }
    // printf("%s", buffer); 
   
    
}
void *threadFunc(void *vargp)
{
    /* pthread_mutex_lock will lock the entire block of code below until the executing thread reaches pthread_mutex_unlock */
//    int lock_success = pthread_mutex_lock(&lock);
//    printf("lock_success = %i\n", lock_success);

    /* pthread_mutex_trylock will make it optional for other threads whether or not wait for the lock to be released */
    // int lock_status = pthread_mutex_trylock(&lock);
    // printf("lock_status = %i\n", lock_status);
    // while (lock_status !=0){  // Trapping the locked thread in a loop until the lock is released.
    //     usleep(100000);  // 0.1s
    //     lock_status = pthread_mutex_trylock(&lock);
    // }
    char reader[500]; 
    char buffer[5]; 
    int client_socket = (int)vargp;
   
    while (1){  // We go into an infinite loop because we don't know how many messages we are going to receive.
        int received_size = recv(client_socket, reader, sizeof(reader), 0);
        char *token;
        char *fileName; 
        char path[100] = "Remote Directory/";
        token = strtok(reader, " ");
        printf("%s\n", token);
        if (received_size == 0 || strcmp("quit", token) == 0){  // Socket is closed by the other end.
            close(client_socket);
            break;
        }
    
        fileName = strtok(NULL, " ");
        strcat(path, fileName); 
        send(client_socket, "hello", 5, 0); 
        if (strcmp("append", token) == 0){
            append(client_socket, path); 
        }
        // if (strcmp("download", token) == 0)
        // {   
        //     recv(client_socket,buffer, sizeof(buffer), 0); 
        //     download(client_socket, "Remote Directory/server_file.txt");
        // }

    }
    
    // recv(client_socket, reader, sizeof(reader), 0); 
    // printf("%s\n", reader);    

    // Release the lock for the next thread.
    // pthread_mutex_unlock(&lock);
    return NULL;
}
int start_server()
{
    
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
    int client_socket;
    // client_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    // if (client_socket < 0) {
    //     perror("accept");
    //     exit(EXIT_FAILURE);
    // }


    ///////////// Start sending and receiving process //////////////
    // char buffer[1024];
    // recv(client_socket, buffer, 1024, 0);
    // download(client_socket, server_socket, "Remote Directory/server_file.txt");
    // printf("here"); 
    // upload(client_socket, "Remote Directory/client_file.txt"); 
    // delete(client_socket, server_socket, "Remote Directory/client_file.txt"); 
    // append(client_socket, server_socket, "Remote Directory/server_file.txt"); 
    // syncheck(client_socket, server_socket, "server_file.txt"); 
    char line[1024];
    // char *token;
    signal(SIGINT, INThandler);
    int count = 0; 
    while(1){     
        client_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_init(&lock, NULL);
        pthread_create(&threads[count], NULL, threadFunc, (void *)client_socket);
        pthread_join(threads[count], NULL);
        pthread_mutex_destroy(&lock);
        close(client_socket);
        count ++; 
    }
    

}

int main(int argc, char *argv[])
{

	// printf("I am the server.\n");
	// printf("Server IP address: %s\n", argv[1]);
	// md5_print();
	// printf("-----------\n");
	start_server(); 
    close(server_socket);
    return 0;
	exit(0);
	return 0;
}