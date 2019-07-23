#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "netdb.h"
#include <time.h>
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
	// Проверка элементов командной строки
  if ((argc < 2) || (argc > 2))  {
    fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
    exit(0);
  }

	// Объявление данных
  int sockfd, newsockfd, noport, n;
  char buffer[255];

  struct sockaddr_in serv_addr , cli_addr;
  socklen_t clilen;

	// Создаем сокет
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    error("Ошибка открытия сокета!");

  bzero((char *) &serv_addr, sizeof(serv_addr));
	// Указываем порт программы с параметра команд строки
  noport = atoi(argv[1]);

	// Указываем тип сокета Интернет
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(noport);

	// Устанавливаем соединение
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("Привязка не удалась.");
  else
    printf("[*] Сервер запущен..\nПрослушиваем %d порт..\n\n", noport);

	// Прослушиваем порт в ожидании ОДНОГО клиента
  listen(sockfd, 1);
  clilen = sizeof(cli_addr);

  // Принимаем запрос на установку соединения
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0)
    error("Ошибка при принятии!");
  if (newsockfd == -1)
    error("прием соединения");
  printf("Сервер: получил соединение от [IP: %s] - [PORT: %d]\n\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

	// Делаем цикл бесконечным
  while (1) {
    bzero(buffer, 255);
	// Принимаем данные
    n = read(newsockfd, buffer, 255);
    if (n < 0)
      error("Ошибка при чтении!");
    timestamp();
    printf("Клиент: %s\a", buffer);

	// Вводим сообщение из консоли
    printf("> ");
    bzero(buffer, 255);
    fgets(buffer, 255, stdin);

	// Отправляем данные
    n = write(newsockfd, buffer, strlen(buffer));
    if (n < 0)
      error("Ошибка при записи!");

	// Прерываем соединения
    int i = strncmp("End", buffer, 3);
    if (i == 0)
      break;
  }

  close(sockfd);
  close(newsockfd);
  printf("\n[~] Вы завершили соединение.\n");

  return 0;
}
