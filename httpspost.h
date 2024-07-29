#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


#define HTTPS_PORT 443
#define MAXIMUM_BUFFER 8192
#define MAXIMUM_POST_BUFFER 8388608 // 8 * 1024 * 1024


void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}


void cleanup_openssl() {
    EVP_cleanup();
}


SSL_CTX* create_ssl_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_client_method(); 
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}


int create_socket_fd(const char* https_server) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "UNABLE TO CREATE A SOCKET\n");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname(https_server);
    if (server == NULL) {
        fprintf(stderr, "Error: Provided http server is INVALID!\n");
        exit(EXIT_FAILURE);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(HTTPS_PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "UNABLE TO CONNECT TO HTTP SERVER!\n");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void Cleanup(int sockfd, SSL* ssl , SSL_CTX* ssl_ctx){
    close(sockfd);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ssl_ctx);
    cleanup_openssl();
}



char* HttpsPost(const char* https_server, const char* url_path, const char* post_data, int* response_code){
    char post_request[MAXIMUM_BUFFER];
    SSL_CTX *ctx;
    SSL *ssl;

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
        https_server,
        strlen(post_data),
        post_data
    );


    init_openssl();
    ctx = create_ssl_context();

    int sockfd = create_socket_fd(https_server);
    
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);


    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    

    if (SSL_write(ssl, post_request, strlen(post_request)) < 0) { // writing data to the socket aka performing a POST request
        fprintf(stderr, "Error: error in writing th GET request to socket, function: HttpsPost\n");
        Cleanup(sockfd, ssl, ctx);
        exit(EXIT_FAILURE);
    }

    char* response_buffer = (char*)malloc(MAXIMUM_BUFFER);
    if (response_buffer == NULL) {
        fprintf(stderr, "Error: error in allocating memory for the request response, function: HttpsPost\n");
        Cleanup(sockfd, ssl, ctx);
        exit(EXIT_FAILURE);
    }

    int total_bytes_read = 0;
    int bytes_read;

    while ((bytes_read = SSL_read(ssl, response_buffer + total_bytes_read, MAXIMUM_BUFFER - total_bytes_read - 1)) > 0) {
        total_bytes_read += bytes_read;
        if (total_bytes_read >= MAXIMUM_BUFFER - 1) {
            break;
        }
    }
 


    if (bytes_read < 0) {
        fprintf(stderr, "Error: error in reading the response from the http server, function: HttpsPost\n");
        free(response_buffer);
        Cleanup(sockfd, ssl, ctx);
        exit(EXIT_FAILURE);
    }

    response_buffer[total_bytes_read] = '\0'; // Null-terminate the response
    sscanf(response_buffer, "HTTP/1.1 %d", &(*response_code)); // get the int response code
    

    Cleanup(sockfd, ssl, ctx);
    return response_buffer;
}