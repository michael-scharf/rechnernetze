#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
  char buffer[1024];
  int sock_fd;
  struct addrinfo hints, *result, *resultsave;
  struct sockaddr_storage server_addr;
  socklen_t server_len = sizeof(server_addr);

  if (argc < 3) {
    fprintf(stderr, "Usage: EchoClientDatagram <hostname> <port>\n"); exit(1);
  }

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM;

  if (getaddrinfo(argv[1], argv[2], &hints, &result) != 0) {
    perror("Error: Unknown host"); exit(1);
  }
  resultsave = result;

  do {
    sock_fd = socket(result->ai_family, result->ai_socktype,
                     result->ai_protocol);
    if (sock_fd >= 0)
      break; /* success */
  } while ((result = result->ai_next) != NULL);

  if (result == NULL) {
    perror("Error: Socket"); exit(1);
  }

  freeaddrinfo(resultsave);

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    if (sendto(sock_fd, buffer, strlen(buffer), 0,
               result->ai_addr, result->ai_addrlen) < 0) {
      perror("Error: SendTo"); exit(1);
    }

    bzero(buffer, sizeof(buffer));

    if (recvfrom(sock_fd, buffer, sizeof(buffer), 0,
                 (struct sockaddr *) &server_addr, &server_len) < 0) {
      perror("Error: RecvFrom"); exit(1);
    }

    printf("Echo: %s", buffer);
  }

  close(sock_fd);
  return 0;
}
