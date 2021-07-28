#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define HOST_NAME "localhost"
#define PORT_NUMBER "80"

#define REQUEST_LEN 4095
#define REQUEST_SIZE REQUEST_LEN + 1

#define RESPONSE_LEN 10000000
#define RESPONSE_SIZE RESPONSE_LEN + 1

#define REQUEST_PREFIX "GET /"

int main(int argc, char *argv[])
{
  char *request = malloc(REQUEST_SIZE);
  char *response = malloc(RESPONSE_SIZE);
  int sock_fd;
  struct addrinfo hints, *result, *resultsave;
  int res, bytes;
  long totalsent, totalrecv;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(HOST_NAME, PORT_NUMBER, &hints, &result) != 0)
  {
    perror("Error: Unknown host");
    exit(1);
  }
  resultsave = result;

  do
  {
    sock_fd = socket(result->ai_family, result->ai_socktype,
                     result->ai_protocol);
    if (sock_fd < 0)
      continue; /* ignore */
    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == 0)
      break; /* success */
    close(sock_fd);
  } while ((result = result->ai_next) != NULL);

  if (result == NULL)
  {
    perror("Error: Socket/Connect");
    exit(1);
  }

  freeaddrinfo(resultsave);

  memset(request, 0, REQUEST_SIZE);
  sprintf(request, REQUEST_PREFIX);
  printf("--- Enter request in format GET /param ...---\n%s", request);
  if (fgets(request + strlen(REQUEST_PREFIX), REQUEST_LEN - strlen(REQUEST_PREFIX) - 2, stdin) == NULL)
  {
    perror("Client error: fgets");
    exit(1);
  }

  /* Replace \n by \r\n */
  bytes = strlen(request);
  if ((bytes > 0) && (request[bytes - 1] == '\n'))
  {
    request[bytes - 1] = '\0';
  }
  bytes = strlen(request);
  request[bytes] = '\r';
  request[bytes + 1] = '\n';

  bytes = 0;
  while (bytes < strlen(request))
  {
    res = write(sock_fd, request + bytes, strlen(request) - bytes);
    if (res < 0)
    {
      perror("Client error: Write");
      exit(1);
    }
    bytes += res;
  }
  totalsent = bytes;

  totalrecv = 0;
  bytes = 0;
  memset(response, 0, RESPONSE_SIZE);
  do
  {
    res = read(sock_fd, response + bytes, RESPONSE_LEN - bytes);
    if (res < 0)
    {
      perror("Client error: Read");
      exit(1);
    }
    else if (res == 0)
    {
      break; /* done */
    }
    printf("%s", response + bytes);
    bytes += res;
    totalrecv += res;
    /* Further actions could be added below */
  } while (bytes < RESPONSE_LEN);

  printf("\n--- Statistics ---\nSent: %ld bytes\nReceived %ld bytes\n", totalsent, totalrecv);

  close(sock_fd);
  free(response);
  free(request);
  return 0;
}
