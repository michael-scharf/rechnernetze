#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
  char buffer[256];
  int bytes;
  int server_fd;
  struct addrinfo hints, *result, *resultsave;
  struct sockaddr_storage client_addr;
  socklen_t client_len = sizeof(client_addr);

  if (argc < 2) {
    fprintf(stderr, "Usage: EchoServerDatagram <port>\n"); exit(1);
  }

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET6; /* v4 and v6 dual stack */
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, argv[1], &hints, &result) != 0) {
    perror("Error: Unknown host"); exit(1);
  }
  resultsave = result;

  do {
    server_fd = socket(result->ai_family, result->ai_socktype, 
                       result->ai_protocol);
    if (server_fd < 0)
      continue; /* ignore */
    if (bind(server_fd, result->ai_addr, result->ai_addrlen) == 0)
      break; /* success */
    close(server_fd); 
  } while ((result = result->ai_next) != NULL);

  if (result == NULL) {
    perror("Error: Socket/Bind"); exit(1);
  }

  freeaddrinfo(resultsave);

  while (1) {
    bzero(buffer, sizeof(buffer));

    bytes = recvfrom(server_fd, buffer, sizeof(buffer), 0, 
                     (struct sockaddr *) &client_addr, &client_len);
    if (bytes < 0) { 
      perror("Error: RecvFrom"); exit(1);
    }

    if (sendto(server_fd, buffer, bytes, 0, 
        (struct sockaddr *) &client_addr, client_len) < 0) {
      perror("Error: SendTo"); exit(1);
    }
  }

  close(server_fd);
  return 0; 
}
