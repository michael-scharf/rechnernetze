#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define PORT_NUMBER "42001"

#define REQUEST_LEN 4095
#define REQUEST_SIZE REQUEST_LEN + 1

#define RESPONSE_LEN 10000000
#define RESPONSE_SIZE RESPONSE_LEN + 1

#define RESPONSE_PREFIX "HTTP/1.0 200 OK\r\nServer: Rechnernetze SimpleServer\r\nContent-type: text/plain\r\n\r\n"
#define RESPONSE_LINE 80
#define RESPONSE_MARK 10
#define RESPONSE_ALIGN 13

void create_response(char *string, int length, int useprefix)
{
  int offset = 0;
  if (useprefix > 0) 
    offset = strlen(RESPONSE_PREFIX);
  if (length < (offset + RESPONSE_ALIGN))
    length = offset + RESPONSE_ALIGN;
  if (length > RESPONSE_LEN)
    length = RESPONSE_LEN;

  memset(string, '.', length);
  if (offset > 0) {
    sprintf(string, "%s", RESPONSE_PREFIX);
    string[offset] = '.'; /* Remove '\0' */
  }

  for (int i = offset; i < length; i++)
  {
    if (((i + 1) % RESPONSE_MARK) == 0)
      string[i] = '_';
    if (((i + 1 - offset) % RESPONSE_LINE) == 0)
    {
      string[i - 1] = '\r';
      string[i] = '\n';
    }
  }
  if (((length - offset - 1) % RESPONSE_LINE) < RESPONSE_ALIGN)
    sprintf(string + length - RESPONSE_ALIGN - 2, "\r\n%010d!\r\n", length);
  else
    sprintf(string + length - RESPONSE_ALIGN, "%010d!\r\n", length);
}

void *client_thread(void *arg)
{
  char *request = malloc(REQUEST_SIZE);
  char *response = malloc(RESPONSE_SIZE);
  int client_fd = *(int *)arg;
  int res, bytes, i, length, iterations, delay, valid;

  memset(request, 0, REQUEST_SIZE);
  bytes = 0;
  do
  {
    res = read(client_fd, request + bytes, REQUEST_LEN - bytes);
    if (res < 0)
    {
      perror("Server error: Read");
      break;
    }
    else if (res == 0)
    {
      break; /* done */
    }
    bytes += res;
  } while ((strchr(request, '\n') == NULL) && (bytes < REQUEST_LEN));

  length = 0;
  iterations = 1;
  delay = 0;
  valid = 0;

  res = sscanf(request, "GET /%d?%d+%d", &length, &iterations, &delay);
  if (res > 0)
  {
    if ((length > 0) && (length <= RESPONSE_LEN) && (iterations > 0) && (delay >= 0))
    {
      valid = 1;
    }
    else
    {
      iterations = 1;
      delay = 0;
      sprintf(response, "HTTP/1.0 400 Bad request\r\n\r\nInvalid number\r\n");
    }
  }
  else
  {
    sprintf(response, "HTTP/1.0 400 Bad request\r\n\r\nSyntax error\r\n");
  }

  for (i = 0; i < iterations; i++) {
    if (valid > 0) create_response(response, length, (i == 0) ? 1 : 0);
    bytes = 0;
    while (bytes < strlen(response))
    {
      res = write(client_fd, response + bytes, strlen(response) - bytes);
      if (res < 0)
      {
        perror("Server error: Write");
        break;
      }
      bytes += res;
    }
    if ((delay > 0) && (i < (iterations-1))) usleep(delay*1000);
  }

  close(client_fd);
  free(response);
  free(request);
  free(arg);
  return NULL;
}

int main(int argc, char *argv[])
{
  int server_fd, client_fd;
  struct addrinfo hints, *result, *resultsave;
  struct sockaddr_storage client_addr;
  socklen_t client_len = sizeof(client_addr);
  pthread_t thread_id;
  int *arg;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET6; /* v4 and v6 dual stack */
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, PORT_NUMBER, &hints, &result) != 0)
  {
    perror("Server error: Unknown host");
    exit(1);
  }
  resultsave = result;

  do
  {
    server_fd = socket(result->ai_family, result->ai_socktype,
                       result->ai_protocol);
    if (server_fd < 0)
      continue; /* ignore */
    if (bind(server_fd, result->ai_addr, result->ai_addrlen) == 0)
      break; /* success */
    close(server_fd);
  } while ((result = result->ai_next) != NULL);

  if (result == NULL)
  {
    perror("Server error: Socket/Bind");
    exit(1);
  }

  freeaddrinfo(resultsave);

  if (listen(server_fd, SOMAXCONN) < 0)
  {
    perror("Server error: Listen");
    exit(1);
  }

  while (1)
  {
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                       &client_len);
    if (client_fd < 0)
    {
      perror("Server error: Accept");
      exit(1);
    }
    arg = malloc(sizeof(int));
    *arg = client_fd;
    if (pthread_create(&thread_id, NULL, client_thread,
                       (void *)arg) < 0)
    {
      perror("Server error: Thread");
      exit(1);
    }
  }

  close(server_fd);
  return 0;
}
