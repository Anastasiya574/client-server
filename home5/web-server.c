#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_SETSOCKETOPT,
    ERR_BIND,
    ERR_LISTEN
};

int init_socket(int port) {
    int server_socket, socket_option = 1;
    struct sockaddr_in server_address;

    //open socket, return socket descriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Fail: open socket");
        exit(ERR_SOCKET);
    }

    //set socket option
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option, (socklen_t) sizeof socket_option);
    if (server_socket < 0) {
        perror("Fail: set socket options");
        exit(ERR_SETSOCKETOPT);
    }

    //set socket address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *) &server_address, (socklen_t) sizeof server_address) < 0) {
        perror("Fail: bind socket address");
        exit(ERR_BIND);
    }

    //listen mode start
    if (listen(server_socket, 5) < 0) {
        perror("Fail: bind socket address");
        exit(ERR_LISTEN);
    }
    return server_socket;
}

char *get_word(int *length) {
    char symbol;
    char *word = (char*)malloc(sizeof(char));
    int i = 0;
    symbol = getchar();
    while ((symbol != ' ') && (symbol != '\n')) {
        word = realloc(word, i + 2);
        word[i] = symbol;
        i++;
        symbol = getchar();
    }
    word[i] = '\0';
    *length = i + 1;
    return word;
}

void my_telnet(char *word) {
    int fd = open(word, O_WRONLY, 0);
    int type_i = 0;
    struct stat data;

    if (fd >= 0) {
        printf("HTTP/1.1 200\n");
        while (word[type_i] != '.') {
            type_i++;
        }
        type_i++;
        printf("content-type: %s/text\n", &(word[type_i]));
        if (lstat(word, &data) == -1) {
            perror("lstat failed");
            exit(EXIT_FAILURE);
        }
        printf("content-length: [%d] \n", (int)(data.st_size));
    } else {
        printf("HTTP/1.1 404\n");
    }
    close(fd);
    puts("\n");
    return;
}

int main(int argc, char** argv) {
    int port, server_socket, client_socket;
    struct sockaddr_in client_address;
    socklen_t size = sizeof client_address;
    const char data[] = "VMK";
    if (argc != 2) {
        puts("Incorrect args.");
        puts("./server <port>");
        puts("Example:");
        puts("./server 5000");
        return ERR_INCORRECT_ARGS;
    }
    port = atoi(argv[1]);
    server_socket = init_socket(port);
    for (;;) {
        puts("Wait for connection");
        client_socket = accept(server_socket,
                              (struct sockaddr *) &client_address,
                              &size);
        printf("connected: %s %d\n", inet_ntoa(client_address.sin_addr),
                                     ntohs(client_address.sin_port));
    char *word = NULL;
    while (strcmp(word, "exit")) {
        free(word);
        int message[12];
        word = NULL;
        char ch;
        read(client_socket, &message , 4);
        read(client_socket, &ch, 1);
        for(int j = 1; ch != 0; j++) {
            word = realloc(word, sizeof(char) * j);
            word[j - 1] = ch;
            read(client_socket, &ch, 1);
        }
        read(client_socket,  &message, 9);
        read(client_socket, &message , 6);
        read(client_socket, &message , 12);
        printf("%s\n", word);
        my_telnet(word);
    }
    close(client_socket);
    }
    return OK;
}
