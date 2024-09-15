#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <unistd.h> 
#include<pthread.h>
#include<regex.h>


#define BUFFER_SIZE 1024
#define PORT 8080
#define SA struct sockaddr

// HTTP server in c

// Handle the each client
// Each client will do operation in diff thread

void *handle_client(void *arg){
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char)); // change the size of max

    // recieve the data from client an store into buffer
    ssize_t bytes_recieved = recv(client_fd, buffer, BUFFER_SIZE, 0);
    
    if(bytes_recieved > 0){

        // check if request is GET
        regex_t regex;
        regcomp(&regex, "^GET /([^ c]*) HTTP/1", REG_EXTENDED);
        regmatch_t matches[2];

        if(regexec(&regex, buffer, 2, matches, 0) == 0){

            // extract the file name from request and decome URL
            buffer[matches[1].rm_eo] = '\0';
            const char *url_encoded_file_name = buffer + matches[1].rm_so;
            char *file_name = url_decode(url_encoded_file_name);

            // get file extension
            char file_text[32];
            strcpy(file_text, get_file_extension(file_name));

            // build HTTP response
            char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
            size_t response_len;
            build_http_response(file_name, file_text, &response_len);

            // send HTTP response to client
            send(client_fd, response, response_len, 0);
            free(response);
            free(file_name);
        }
        regfree(&regex);
    }
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}

int main(int argc, char *argv[]){

    // HTTP is built on TCP
    // I need to save multiple connection communicating to server
    // in some kind of Array or something inbuilt
    // server will create a thread to handle a connection
    // the current main thread which server is running should 
    // not get blocked

    int socket_fd;
    struct sockaddr_in server_addr;


    // create a socket for server
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        printf("Socket connection failed...\n");
        exit(EXIT_FAILURE);
    }else{
        printf("Socket created successfully...\n");
    }

    // Assign host and port to server
    server_addr.sin_family = AF_INET; // AF_INET is for IPv4 Network address scheme
    server_addr.sin_addr.s_addr = INADDR_LOOPBACK; // Localhost
    server_addr.sin_port = htons(PORT);

    // Bind the socket_fd to Host and port
    int is_bind = bind(socket_fd, (SA *)&server_addr, sizeof(server_addr));
    if(is_bind < 0){
        printf("Socket bind failed...\n");
        exit(EXIT_FAILURE);
    }else{
        printf("Socket successfully binded...\n");
    }

    // Listen for connection on localhost and 8080 port
    int listen_status = listen(socket_fd, 5);
    if(listen_status < 0){
        printf("Listening failed...\n");
        exit(EXIT_FAILURE);
    }else{
        printf("Server is listening on port: %d\n", PORT);
    }

    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int)); // (int *) for cpp
        // int client_fd;

        // Accept the client connection
        *client_fd = accept(socket_fd, (SA*)&client_addr, &client_addr_len);
        if((*client_fd) < 0){
            printf("Server is failed to accept the connection\n");
            continue;
        }else{
            printf("Connection established...\n");
        }

        // Now create a new thread to handle the client request and response
        // So server will be free to listen other connection concurrently
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        pthread_detach(thread_id);
    }
    close(socket_fd);
    return 0;
}