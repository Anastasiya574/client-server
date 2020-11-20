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

enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_CONNECT
};

int init_socket(const char *ip, int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *host = gethostbyname(ip);
    struct sockaddr_in server_address;

    //open socket, result is socket descriptor
    if (server_socket < 0) {
        perror("Fail: open socket");
        exit(ERR_SOCKET);
    }

    //prepare server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    memcpy(&server_address.sin_addr, host -> h_addr_list[0],
           (socklen_t) sizeof server_address.sin_addr);

    //connection
    if (connect(server_socket, (struct sockaddr*) &server_address,
        (socklen_t) sizeof server_address) < 0) {
        perror("Fail: connect");
        exit(ERR_CONNECT);
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

int main(int argc, char **argv) {
    char *ip;
    int port, server;
    char *word = NULL;
    int word_len = 0;
    if (argc != 3) {
        puts("Incorrect args.");
        puts("./client <ip> <port>");
        puts("Example:");
        puts("./client 127.0.0.1 5000");
        return ERR_INCORRECT_ARGS;
    }
    ip = argv[1];
    port = atoi(argv[2]);
    server = init_socket(ip, port);
    word = get_word(&word_len);
    while (strcmp(word, "exit")){
            write(server, "GET\0", 4);
            write(server, word, word_len);
            write(server, "HTTP/1.1\0", 9);
            write(server, "Host:\0", 6);
            write(server, "mymath.info\0", 12);
            free(word);
            word = get_word(&word_len);
    }
    free(word);
    close(server);
    return OK;
}
