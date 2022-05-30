#include <stdio.h>
#include <string.h>

int main(){
    //opens the file
    FILE *input_file = fopen("user_commands.txt", "r");
    char line[256];
    //reads the file line by line
    while (fgets(line, sizeof(line), input_file)) {
        //extracts first token in line (i.e. the command)
        char *token = strtok(line, " ");
        //send different message for each command 
        if (token = "append") {
            //send and receive an entire file
            //send_append(client_socket, file)
        }
        else if (token = "upload") {
            //send_upload(client_socket, file)
        }
        else if (token = "download") {
            //send_download(client_socket, file)
        }
        else if (token = "delete") {
            //send_delete(client_socket, file)
        }
        else if (token = "syncheck") {
            //send_append(client_socket, file)
        }
        else if (token = "quit") {
            //quit
        }
    }

}

