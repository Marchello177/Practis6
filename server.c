#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <time.h> 
 
#define BUFFER_SIZE 1024 
 
// Функция для обработки клиента 
void handle_client(int client_socket, struct sockaddr_in client_addr) { 
    char buffer[BUFFER_SIZE]; 
    int random_number = rand() % 100 + 1;  // Генерация случайного числа от 1 до 100 
    int guess; 
    int attempts = 0; 
 
    printf("Новое подключение от %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
 
    while (1) { 
        memset(buffer, 0, BUFFER_SIZE); 
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);  // Получение данных от клиента 
        if (bytes_received <= 0) { 
            printf("Клиент %s:%d отключился\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
            close(client_socket); 
            return; 
        } 
 
        guess = atoi(buffer);  // Преобразование полученных данных в число 
        attempts++; 
 
        if (guess < random_number) { 
            send(client_socket, "МАЛО", 8, 0);  // Отправка ответа "МАЛО" 
            printf("%s:%d: Предположение %d слишком мало\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), guess); 
        } else if (guess > random_number) { 
            send(client_socket, "ВЕЛИКО", 13, 0);  // Отправка ответа "ВЕЛИКО" 
            printf("%s:%d: Предположение %d слишком велико\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), guess); 
        } else { 
            send(client_socket, "ПОБЕДА", 13, 0);  // Отправка ответа "ПОБЕДА" 
            printf("%s:%d: Предположение %d верно! Всего попыток: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), guess, attempts); 
            close(client_socket); 
            return; 
        } 
    } 
} 
 
int main(int argc, char *argv[]) { 
    if (argc != 2) { 
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]); 
        exit(1); 
    } 
 
    int port = atoi(argv[1]); 
    int server_socket, client_socket; 
    struct sockaddr_in server_addr, client_addr; 
    socklen_t client_addr_len = sizeof(client_addr); 
 
    srand(time(NULL));  // Инициализация генератора случайных чисел 
 
    server_socket = socket(AF_INET, SOCK_STREAM, 0);  // Создание сокета 
    if (server_socket < 0) { 
        perror("Ошибка создания сокета"); 
        exit(1); 
    } 
 
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(port); 
 
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {  // Привязка сокета к адресу и порту 
        perror("Ошибка привязки сокета"); 
        close(server_socket); 
        exit(1); 
    } 
 
    if (listen(server_socket, 5) < 0) {  // Начало прослушивания сокета 
        perror("Ошибка прослушивания сокета"); 
        close(server_socket); 
        exit(1); 
    } 
 
    printf("Сервер слушает на порту %d\n", port); 
 
    while (1) { 
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);  // Принятие подключения от клиента 
        if (client_socket < 0) { 
            perror("Ошибка принятия подключения"); 
            continue; 
        } 
 
        handle_client(client_socket, client_addr);  // Обработка клиента 
    } 
 
    close(server_socket); 
    return 0; 
}
