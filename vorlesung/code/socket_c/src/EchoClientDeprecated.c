#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
  char buffer[256];
  int sock_fd, port_number;
  struct sockaddr_in server_addr;
  struct hostent *server;

  if (argc < 3) {
    fprintf(stderr, "Usage: EchoClientDeprecated <hostname> <port>\n"); exit(1);
  }

  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr, "Error: Unknown host\n"); exit(1);
  }

  port_number = atoi(argv[2]);

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET; /* IPv4 only */
  bcopy(server->h_addr, &server_addr.sin_addr.s_addr, server->h_length);
  server_addr.sin_port = htons(port_number);

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("Error: Socket"); exit(1);
  }

  if (connect(sock_fd, (struct sockaddr *) &server_addr,
              sizeof(server_addr)) < 0) {
    perror("Error: Connect"); exit(1);
  }

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    if (write(sock_fd, buffer, strlen(buffer)) < 0) {
      perror("Error: Write"); exit(1);
    }

    bzero(buffer, sizeof(buffer));

    if (read(sock_fd, buffer, sizeof(buffer)) < 0) {
      perror("Error: Read"); exit(1);
    }

    printf("Echo: %s", buffer);
  }

  close(sock_fd);
  return 0;
}
