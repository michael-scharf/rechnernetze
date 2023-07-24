#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
  char buffer[256];
  int sock_fd;
  struct addrinfo hints, *result, *resultsave;

  if (argc < 3) {
    fprintf(stderr, "Usage: EchoClient <hostname> <port>\n"); exit(1);
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(argv[1], argv[2], &hints, &result) != 0) {
    perror("Error: Unknown host"); exit(1);
  }
  resultsave = result;

  do {
    sock_fd = socket(result->ai_family, result->ai_socktype,
                     result->ai_protocol);
    if (sock_fd < 0)
      continue; /* ignore */
    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == 0)
      break; /* success */
    close(sock_fd);
  } while ((result = result->ai_next) != NULL);

  if (result == NULL) {
    perror("Error: Socket/Connect"); exit(1);
  }

  freeaddrinfo(resultsave);

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    if (write(sock_fd, buffer, strlen(buffer)) < 0) {
      perror("Error: Write"); exit(1);
    }

    memset(buffer, 0, sizeof(buffer));

    if (read(sock_fd, buffer, sizeof(buffer)) < 0) {
      perror("Error: Read"); exit(1);
    }

    printf("Echo: %s", buffer);
  }

  close(sock_fd);
  return 0;
}
