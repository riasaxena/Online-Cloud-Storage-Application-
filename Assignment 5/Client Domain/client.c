// The 'client.c' code goes here.
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Client Domain'.
#define PORT 9999
char junk[10];

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

int upload(int client_socket, char source_path[], char fileName[]) {
    FILE *fptr;
    fptr = fopen(source_path,"r"); 
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
        printf("%d bytes uploaded successfully.\n", file_size); 
    }
    else{
        send (client_socket, "invalid", 7, 0); 
        printf("File [%s] could not be found in local directory\n", fileName); 
    }
    return 0;
 }

int download(int client_socket, char source_path[], char fileName[]){
    char error[8];
    char line[500]; 
    recv(client_socket, error, 10, 0); 
    // printf("%s, %d\n", error), strncmp(error, "valid", 5); 
    // Opening a new file in write-binary mode to write the received file bytes into the disk using fptr.
    if (strncmp(error, "valid", 5) == 0){    
         FILE *fptr;
        fptr = fopen(source_path,"w");
        while (1){
            send(client_socket, "junk", sizeof("junk"), 0); 
            recv(client_socket, line, sizeof(line), 0);
            // printf("%s", line); 
            if (strncmp(line, "quit", 4) == 0){
                int file_size = ftell(fptr); 
                printf("%d bytes downloaded successfully.\n",file_size);
                fclose(fptr);
                break;
            }
            fwrite(line, sizeof(char), strlen(line), fptr);
        }
        
    }
    else{
        printf("File [%s] could not be found in remote directory.\n", fileName); 
    }
    // printf("%d\n", chunk_counter);
}
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
int syncheck_download(int client_socket, char source_path[], char fileName[]){
    char error[8];
    char line[500]; 
    recv(client_socket, error, 10, 0); 
    // printf("%s, %d\n", error), strncmp(error, "valid", 5); 
    // Opening a new file in write-binary mode to write the received file bytes into the disk using fptr.
    if (strncmp(error, "valid", 5) == 0){    
         FILE *fptr;
        fptr = fopen(source_path,"w");
        while (1){
            send(client_socket, "junk", sizeof("junk"), 0); 
            recv(client_socket, line, sizeof(line), 0);
            // printf("%s", line); 
            if (strncmp(line, "quit", 4) == 0){
                int file_size = ftell(fptr); 
                // printf("%d bytes downloaded successfully.\n",file_size);
                fclose(fptr);
                break;
            }
            fwrite(line, sizeof(char), strlen(line), fptr);
        }
        
    }
    else{
        // printf("File [%s] could not be found in remote directory.\n", fileName); 
    }
    // printf("%d\n", chunk_counter);
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
    recv(client_socket, inRemote, sizeof(inRemote), 0); 
    // printf("inRemote %s", inRemote); 
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

void readFile(char filename[], int client_socket) {
    FILE *input_file = fopen(filename, "r");
    char line[500];
    char buffer [5]; 
    pthread_mutex_t mutexLock = pthread_mutex_init(&lock, NULL);
    while (fgets(line, sizeof(line), input_file)) {
        char *token;
        char *fileName; 
        char path[100] = "Local Directory/";
        printf(">%s", line); 
        send(client_socket, line, sizeof(line), 0); 
        sleep(1); 
        // printf("s %s", line);
        token = strtok(line, " ");
        if (strcmp("quit", token) == 0){
            printf("\n"); 
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
                sleep(1); 
            }
            if (strcmp("upload", token) == 0){
                upload(client_socket, path, fileName); 
                sleep(1); 
            }
            if (strcmp("download", token) == 0){
                download(client_socket, path, fileName); 
            }
            if (strcmp("delete", token) == 0){
                delete(client_socket, fileName); 
            }
            if (strcmp("syncheck", token) == 0){
                syncheck(client_socket, fileName); 
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