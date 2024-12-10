#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3000
#define BUFFER_SIZE 1024

int main(void)
{
  int server_fd, client_addr_len;
  struct sockaddr_in client_addr;
  char buffer[BUFFER_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd != 0)
  {
    printf("Socket creation failed: %s\n", strerror(errno));
    return 1;
  }

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(PORT),
      .sin_addr = {htonl(INADDR_ANY)}};

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    printf("Bind failed: %s\n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0)
  {
    printf("Listen failed: %s\n", strerror(errno));
    return 1;
  }

  printf("Server listening on port %d\n", PORT);

  while (1)
  {
    client_addr_len = sizeof(client_addr);

    int fd = accept(server_fd, (struct sockaddr *)&server_addr, &client_addr_len);
    if (fd == -1)
    {
      printf("Accept failed: %s\n", strerror(errno));
      return 1;
    }

    printf("Client connected!!!\n");

    ssize_t bytes_read = read(server_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1)
    {
      printf("Read failed: %s\n", strerror(errno));
      close(fd);
      return 1;
    }

    buffer[bytes_read] = '\0';

    printf("Received request: \n%s\n", buffer);

    char *request = strtok(buffer, "\r\n");
    if (!request)
    {
      printf("Failed to parsed request");
      return 1;
    }

    char *method = strtok(request, " ");
    if (!method)
    {
      printf("Failed to parsed method");
      return 1;
    }

    char *url_path = strtok(NULL, " ");
    if (!url_path)
    {
      printf("Failed to parsed url path");
      return 1;
    }

    char *reply;

    if (strcmp(url_path, "/") == 0)
    {
      reply = "HTTP/1.1 200 OK\r\n\r\n";
    }
    else
    {
      reply = "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    send(fd, reply, strlen(reply), 0);
    
    close(fd);
  }

  return 0;
}
