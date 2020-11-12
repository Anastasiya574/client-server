#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_SETSOCKETOPT,
    ERR_BIND,
    ERR_LISTEN
};

int init_socket(int port) {
    // open socket, return socket descriptor
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Fail: open socket");
        _exit(ERR_SOCKET);
    }

    // set socket option
    int socket_option = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option,
              (socklen_t) sizeof(socket_option));
    if (server_socket < 0) {
        perror("Fail: set socket options");
        _exit(ERR_SETSOCKETOPT);
    }

    // set socket address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *) &server_address,
       (socklen_t) sizeof(server_address)) < 0) {
        perror("Fail: bind socket address");
        _exit(ERR_BIND);
    }

    // listen mode start
    if (listen(server_socket, 5) < 0) {
        perror("Fail: bind socket address");
        _exit(ERR_LISTEN);
    }
    return server_socket;
}


int main(int argc, char** argv) {
    if (argc != 3) {
        puts("Incorrect args.");
        puts("./server <port> <number_of_clients>");
        puts("Example:");
        puts("./server 5000 1");
        return ERR_INCORRECT_ARGS;
    }
    int port = atoi(argv[1]);
    int number_of_clients = atoi(argv[2]);
    int server_socket = init_socket(port);
    puts("Wait for connection");
    struct sockaddr_in client_address;
    socklen_t size;
    int *client_socket = malloc(number_of_clients * sizeof(int));
    for (int i = 0; i < number_of_clients; i++) {
        client_socket[i] = accept(server_socket,
                                (struct sockaddr *) &client_address,
                                &size);
        printf("connected: %s %d\n", inet_ntoa(client_address.sin_addr),
                                ntohs(client_address.sin_port));
    }
    int i = 0;
    char ch;
    int ret_size = 1;
    do {
        for (int i = 0; i < number_of_clients; i++) {
            ret_size = read(client_socket[i], &ch, 1);
            printf("%d: ", i + 1);
            putchar(ch);
            puts("");
        }
    } while (ret_size > 0);
    for (int i = 0; i < number_of_clients; i++) {
        close(client_socket[i]);
    }
    free(client_socket);
    close(server_socket);
    return OK;
}
