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
pthread_t threads[15];
// pthread_t thread;
int global_variable = 5;
pthread_mutex_t lock;
int server_socket;
char junk[10];
void INThandler(int);

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     close(server_socket);
     exit(0); 
}

int append(int client_socket, char destination_path[]){
    FILE *fptr;
    char line [500]; 
    fptr = fopen(destination_path,"a");
    int received_size;
    if (fptr){  
        send(client_socket, "valid", 5, 0); 
        // sleep (1);  
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
                // printf("%s", line); 
                fwrite(line, sizeof(char), strlen(line), fptr);
            }
        } 
        fclose(fptr); 
    }
    else{
        send(client_socket, "invalid", 7, 0); 
    }

}

int upload(int client_socket, char destination_path[]){
    char error[8];
    char line[500]; 
    recv(client_socket, error, 10, 0); 
    // Opening a new file in write-binary mode to write the received file bytes into the disk using fptr.
    if (strncmp(error, "valid", 5) == 0){    
        FILE *fptr;
        fptr = fopen(destination_path,"w");

        while (1){
            send(client_socket, "junk", sizeof("junk"), 0); 
            recv(client_socket, line, sizeof(line), 0);
            if (strncmp(line, "quit", 4) == 0){
                fclose(fptr); 
                break; 
            }
            fwrite(line, sizeof(char), strlen(line), fptr);
        }
    }
    return 0; 
}
int download(int client_socket, char destination_path[]) {
    FILE *fptr;
    fptr = fopen(destination_path,"r"); 
    char line[500]; 
    if (fptr){
        ssize_t sent_bytes;
        int total_bytes = 0;
        fseek(fptr, 0L, SEEK_END);  
        int file_size = ftell(fptr); 
        fseek(fptr, 0L, SEEK_SET);  
        send(client_socket, "valid", 5, 0); 
        while (fgets(line, sizeof(line), fptr)) {
            recv(client_socket, junk, sizeof(junk),0); 
            sent_bytes = send(client_socket, line, sizeof(line), 0); 
        }
        recv(client_socket, junk, sizeof(junk),0);
        send(client_socket, "quit", 4, 0);
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

int syncheck(int client_socket, char path[]){
    FILE *fptr;
    char buffer [100];
    int file_size = 0; 
    // printf("%s\n",path); 
    fptr = fopen(path,"rb"); 
    // printf("1\n"); 
    // printf("%d, %s\n", fptr, path); 
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
        // printf("reader: %s\n", reader); 
        token = strtok(reader, " ");
        
        if (strcmp("quit", token) == 0){  // Socket is closed by the other end.
            break;
        }
        fileName = strtok(NULL, "\n");
        strcat(path, fileName); 
        // printf("%d\n", strcmp("append", token));
        if (strcmp("append", token) == 0){
            append(client_socket, path); 
        }
        if (strcmp("upload", token) == 0){
            upload(client_socket, path);
        }
        if (strcmp("download", token) == 0){
            download(client_socket, path);
        }
        if (strcmp("delete", token) == 0){
            delete(client_socket, path);
        }
        if (strcmp("syncheck", token) == 0){
            // printf("2"); 
            syncheck(client_socket, path);
        }
        
    }
    close(client_socket); 
    
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
	start_server(); 
    close(server_socket);
    return 0;
	exit(0);
	return 0;
}