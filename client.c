#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
 
#define BUFFER_SIZE 1024 
 
int main(int argc, char *argv[]) { 
    if (argc != 3) { 
        fprintf(stderr, "использование: %s <адрес_сервера> <порт_сервера>\n", argv[0]); 
        exit(1); 
    } 
 
    char *server_ip = argv[1]; 
    int server_port = atoi(argv[2]); 
    int client_socket; 
    struct sockaddr_in server_addr; 
    char buffer[BUFFER_SIZE]; 
    int guess; 
 
    client_socket = socket(AF_INET, SOCK_STREAM, 0);  // Создание сокета 
    if (client_socket < 0) { 
        perror("ошибка создания сокета"); 
        exit(1); 
    } 
 
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = inet_addr(server_ip); 
    server_addr.sin_port = htons(server_port); 
 
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {  // Подключение к серверу 
        perror("ошибка подключения к серверу"); 
        close(client_socket); 
        exit(1); 
    } 
 
    printf("подключено к серверу %s:%d\n", server_ip, server_port); 
 
    while (1) { 
        printf("введите ваше предположение (1-100): "); 
        scanf("%d", &guess); 
 
        sprintf(buffer, "%d", guess); 
        send(client_socket, buffer, strlen(buffer), 0);  // Отправка предположения серверу 
 
        memset(buffer, 0, BUFFER_SIZE); 
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);  // Получение ответа от сервера 
        if (bytes_received <= 0) { 
            printf("сервер отключился\n"); 
            break; 
        } 
 
        if (strcmp(buffer, "ПОБЕДА") == 0) { 
            printf("поздравляем! вы угадали число!\n"); 
            break; 
        } else if (strcmp(buffer, "МАЛО") == 0) { 
            printf("слишком мало. попробуйте еще раз.\n"); 
        } else if (strcmp(buffer, "ВЕЛИКО") == 0) { 
            printf("слишком велико. попробуйте еще раз.\n"); 
        } else { 
            printf("неизвестный ответ от сервера: %s\n", buffer); 
        } 
    } 
 
    close(client_socket); 
    return 0; 
}
