#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
  char buffer[256];
  int bytes;
  int server_fd, client_fd, port_number;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);

  if (argc < 2) {
    fprintf(stderr, "Usage: EchoServerDeprecated <port>\n"); exit(1);
  }

  port_number = atoi(argv[1]);

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET; /* IPv4 only */
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_number);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Error: Socket"); exit(1);
  }

  if (bind(server_fd, (struct sockaddr *) &server_addr,
           sizeof(server_addr)) < 0)  {
    perror("Error: Bind"); exit(1);
  }

  if (listen(server_fd, SOMAXCONN) < 0) {
    perror("Error: Listen"); exit(1);
  }

  client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);
  if (client_fd < 0) {
    perror("Error: Accept"); exit(1);
  }

  while (1) {
    bytes = read(client_fd, buffer, sizeof(buffer));
    if (bytes < 0) {
      perror("Error: Read"); exit(1);
    }
    if (!bytes) break; /* done */

    if (write(client_fd, buffer, bytes) < 0) {
      perror("Error: Write"); exit(1);
    }
  }

  close(client_fd);
  close(server_fd);
  return 0;
}
