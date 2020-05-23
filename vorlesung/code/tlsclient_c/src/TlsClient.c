#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>

int main(int argc, char *argv[])
{
  char response[4096];
  int sock_fd, n;
  struct addrinfo hints, *result, *resultsave;
  SSL_CTX *ctx;
  SSL *ssl;

  char hostname[] = "www.hs-esslingen.de";
  char port[] = "443";
  char request[] = "GET / HTTP/1.1\r\nHost: www.hs-esslingen.de\r\n" \
                   "Connection: close\r\n\r\n";

  SSL_library_init();
  ctx = SSL_CTX_new(TLSv1_2_client_method());
  if (!ctx) { fprintf(stderr, "Error: SSL context"); exit(1); }

  /* Load trusted certificates (tested on CentOS Linux) */
  if (!SSL_CTX_load_verify_locations(ctx, "/etc/pki/tls/cert.pem", NULL)) {
    fprintf(stderr, "Error: Trust store\n"); exit(1);
  }

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(hostname, port, &hints, &result) != 0) {
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

  ssl = SSL_new(ctx);
  if (!ssl) { fprintf(stderr, "Error: SSL structure"); exit(1); }
  SSL_set_fd(ssl, sock_fd);

  SSL_connect(ssl); /* TLS handshake */

  if (SSL_get_verify_result(ssl) != X509_V_OK) { /* Verify certificate */
    fprintf(stderr, "Error: Server certificate verification\n"); exit(1);
  }

  if (SSL_write(ssl, request, strlen(request)) < strlen(request)) {
    fprintf(stderr, "Error: Write\n"); exit(1);
  }

  do {
    bzero(response, sizeof(response));
    if ((n = SSL_read(ssl, response, sizeof(response))) < 0) {
      fprintf(stderr, "Error: Read\n"); exit(1);
    }
    if (n > 0) printf("%s", response);
  } while (n > 0);

  SSL_free(ssl);
  close(sock_fd);
  SSL_CTX_free(ctx);
  return 0;
}
