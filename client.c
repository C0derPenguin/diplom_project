#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "netdb.h"
#include "time.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"

void error(const char *msg);
int timestamp();

// Обработка ошибок
void error(const char *msg) {
  perror(msg);
  exit(0);
} 

int timestamp() {
   time_t now;
   struct tm *time_struct;
   int length;
   char time_buffer[40];

   time(&now);
   time_struct = localtime((const time_t *)&now);
   length = strftime(time_buffer, 40, "[%H:%M:%S] ", time_struct);
   printf(time_buffer, length);
}

int main(int argc, char *argv[]) {

// Объявление данных
  int sockfd, noport, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[255];

// Проверка элементов командной строки
  if ((argc < 3) || (argc > 3))  {
    fprintf(stderr, "Usage: %s <hostname> <port number>\n", argv[0]);
    exit(0);
  }

// Указываем порт программы с параметра команд строки
  noport = atoi(argv[2]);

// Создаем сокет
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    error("Ошибка открытия сокета!");

// Конвертируем имя хоста в IP-адрес
  if ((server = gethostbyname(argv[1])) == NULL)
    error("Ошибка, нет такого хоста!");

// Указываем тип сокета Интернет
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

// Указываем порт сокета
  serv_addr.sin_port = htons(noport);

// Устанавливаем соединение
  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("Ошибка соединения");
  else
    printf("[~] Вы успешно подключились!\n\n");

// Выводим цикл
  while (1) {

// Вводим сообщение из консоли
    printf("> ");
    bzero(buffer, 255);
    fgets(buffer, 255, stdin);

// Отправляем данные
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
      error("Ошибка при записи!");
    bzero(buffer, 255);

// Принимаем данные
    n = read(sockfd, buffer, 255);
    if (n < 0)
      error("Ошибка при чтении!");
    timestamp();
    printf("Сервер: %s\a", buffer);

// Прерываем соединения
    int i = strncmp("End", buffer, 3);
    if (i == 0)
      break;
  }

  close(sockfd);
  printf("\nСервер завершил соединение.\n");

  return 0;
}
