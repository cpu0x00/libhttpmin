#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>



#define HTTP_PORT 80
#define MAXIMUM_BUFFER 8192


int create_socket_fd(const char* http_server) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "UNABLE TO CREATE A SOCKET\n");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname(http_server);
    if (server == NULL) {
        fprintf(stderr, "Error: Provided http server is INVALID!\n");
        exit(EXIT_FAILURE);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(HTTP_PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "UNABLE TO CONNECT TO HTTP SERVER!\n");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}



char* HttpPost(const char* http_server, const char* url_path, const char* post_data, int* response_code){

    char post_request[MAXIMUM_BUFFER];


    snprintf(
        post_request, 
        sizeof(post_request), 
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n" // <--- change here if needed
        "Content-Length: %zu\r\n"
        "Connection: Close\r\n\r\n"
        "%s",
        url_path,
        http_server,
        strlen(post_data),
        post_data
    );

    

    int sockfd = create_socket_fd(http_server);
    

    if (write(sockfd, post_request, strlen(post_request)) < 0) { // writing data to the socket aka performing a POST request
        fprintf(stderr, "Error: error in writing th POST request to socket, function: HttpPost\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    char* response_buffer = (char*)malloc(MAXIMUM_BUFFER);
    if (response_buffer == NULL) {
        fprintf(stderr, "Error: error in allocating memory for the request response, function: HttpPost\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int total_bytes_read = 0;
    int bytes_read;

    while ((bytes_read = read(sockfd, response_buffer + total_bytes_read, MAXIMUM_BUFFER - total_bytes_read - 1)) > 0) {
        total_bytes_read += bytes_read;
        if (total_bytes_read >= MAXIMUM_BUFFER - 1) {
            break;
        }
    }
 


    if (bytes_read < 0) {
        fprintf(stderr, "Error: error in reading the response from the http server, function: HttpPost\n");
        free(response_buffer);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    response_buffer[total_bytes_read] = '\0'; // Null-terminate the response
    sscanf(response_buffer, "HTTP/1.1 %d", &(*response_code)); // get the int response code
    

    close(sockfd);
    return response_buffer;


    
}