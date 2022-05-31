// The 'client.c' code goes here.
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include "Md5.c"  // Feel free to include any other .c files that you need in the 'Client Domain'.
#define PORT 9999
int upload(int client_socket, char source_path[]) {
    char buffer[1024];
    recv(client_socket, buffer, 1024, 0 );
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
    printf("%d bytes uploaded successfully.\n", total_bytes); 
    // close(client_socket);
    // close(server_socket);
    fclose(fptr);
    return 0;
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
void readFile(const char * filename, int client_socket) {
    
    FILE *input_file = fopen(filename, "r");
    char line[500];
    
    // char line = "download";
     //reads the file line by line
    while (fgets(line, sizeof(line), input_file)) {
        strcat(line, " ");  
        char *token;
        printf("%s\n", line); 
        
        token = strtok(line, " ");
        if (strcmp("pause", token) == 0){
        //  as discussed before, your application should be able to handle multiple users at 
        // the same time. Since the user behavior is read from a file instead of taking user input in real-time, 
        // an entire scenario of a single user (a single text file of commands) would be executed in a fraction 
        // of a second. This behavior would make it difficult to test the concurrency aspects of multithreading 
        // in  the  code.  Therefore,  your  code  should  support  a  very  simple  "pause  <time>"  command  that 
        // would pause the execution of the client code for <time> seconds.

        }
        else if (strcmp("append", token) == 0){
        // the  user  can  type  “append  <file_name>”  to  start  string  appending  mode  on  the  file 
        // “file_name” located in the remote directory. If <file_name> is not found in the remote directory, 
        // the application prints “File <file_name> could not be found in remote directory.”. Otherwise, the 
        // application enters appending mode and the prompt is changed from “> ” to “Appending> “ 
            
        }
        else if (strcmp("upload", token) == 0){ 
        // to upload a file from the local directory to the remote directory, the user types “upload 
        // <file_name>”.  If  the  file  doesn’t  exist  in  the  user’s  local  directory,  the  application  prints  “File 
        // <file_name> could not be found in local directory.”. Otherwise, the application uploads the file from 
        // the user’s local folder to the remote folder via a TCP socket. Then the application prints a success 
        // message that indicates the file size as shown below. 
 
            
        }
        else if (strcmp("download", token) == 0){
        // to  download  a  file  from  the  remote  directory  to  the  local  directory,  the  user  types 
        // “download <file_name>”. If the file doesn’t exist in the user’s remote directory on the server, the 
        // application  prints  “File  <file_name>  could  not  be  found  in  remote  directory.”.  Otherwise,  the 
        // application  downloads  the  file  via  a  TCP  socket  from  the  user’s  remote  directory  to  the  local 
        // directory. Then the application prints a success message that indicates the file size as shown below.  
 
        }
         else if (strcmp("delete", token) == 0){
        // the user can type “delete <file_name>” command to delete <file_name> on the remote 
        // directory. If the file doesn’t exist in the remote directory on the server, the application prints “File 
        // <file_name> could not be found in remote directory.”. Otherwise, the server deletes the file from 
        // the remote directory and the client terminal prints a success message.  
            ; 
        }
        else if (strcmp("syncheck", token) == 0){
        // the  user  can  type  “syncheck  <file_name>”  command  to  get  a  short  report  about 
        // “file_name”, whether on the local or remote directory.  
        }
        else if (strcmp("quit", token) == 0){
        // the  user  can  type  “quit”  command  to  close  the  client  socket  then  terminate  the  client 
        // application (but not the server). To quit the server, you should do “ctrl+c” on the server terminal 
        // which  would  cause  the  server  to  close  all  active  client  sockets  as  well  as  the  server  socket,  then 
        // terminate the server application. 
        }


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
    char message[] = "download server_file.txt"; 
    send(client_socket, message, strlen(message), 0);
    // upload(client_socket, "Local Directory/client_file.txt");
    // download(client_socket, "Local Directory/server_file.txt");
   
    close(client_socket);
    


}

int main(int argc, char *argv[])
{

	// printf("I am the client.\n");
	// printf("Input file name: %s\n", argv[1]);
	// printf("My server IP address: %s\n", argv[2]);
	// md5_print();
	// printf("-----------\n");
    char const* const fileName = "user_command.txt";
	start_client(fileName, argv[2]); 
	exit(0);
	return 0;
}