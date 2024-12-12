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
  char buffer[BUFFER_SIZE];

  // Create server file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(PORT),
    .sin_addr = { htonl(INADDR_ANY) }
  };

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("Bind failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) < 0) {
    printf("Listen failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  int addrlen = sizeof(server_addr);

  printf("Server listening on port %d\n", PORT);


  while (1) {
    int fd = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen);
    if (fd < 0) {
      printf("Failed to accept connection: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
      printf("Failed to read request: %s\n", strerror(errno));
      close(fd);
      exit(EXIT_FAILURE);
    }

    buffer[bytes_read] = '\0';
    printf("Received request: \n%s\n", buffer);
    // extract request
    char *request_line = strtok(buffer, "\r\n");
    char *host_line = strtok(NULL, "\r\n");
    char *user_agent_line = strtok(NULL, "\r\n");

    // extract method, url path, http version
    char *method = strtok(request_line, " ");
    char *url_path = strtok(NULL, " "); 
    char *http_ver = strtok(NULL, " ");


    // extract host
    char *host = strtok(host_line, " ");
    host = strtok(NULL, " ");
    printf("Host: %s\n", host);

    // extract user agent
    char *user_agent = strtok(user_agent_line, " ");
    user_agent = strtok(NULL, " ");
    printf("User-Agent: %s\n", user_agent);

    char *response;

    if (strcmp(url_path, "/") == 0) {
      response = "HTTP/1.1 200 OK\r\n\r\n";
    } else {
      response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }
    
    write(fd, response, strlen(response));

    close(fd);
  }
  return 0;
}
