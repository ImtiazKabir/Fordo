#ifndef SERVER_PROT_H_
#define SERVER_PROT_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

struct Server {
  char const *ip_address;
  unsigned short port;
  int socket;
  int client_socket;
  struct sockaddr_in socket_address;
  unsigned int socket_address_len;
  void *handler_chain;
};

#endif /* SERVER_PROT_H_ */
