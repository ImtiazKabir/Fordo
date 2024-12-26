#include "server.h"

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "imlib/imstdinc.h"
#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/ansi.h"
#include "imlib/imparam.h"
#include "imlib/impanic.h"
#include "imlib/imerror.h"
#include "imlib/imresult.h"
#include "imlib/imodlog.h"
#include "imlib/imlog.h"
#include "imlib/imstr.h"

#include "../request/request.h"
#include "../response/response.h"

#include "../http_status/http_status.h"
#include "../http_mimes/http_mimes.h"

IM_DEFINE_ERROR(ServerError, SERR_ERR, "Internal server error")
IM_DEFINE_ERROR(ListenError, SERR_LISTEN, "Listen error")
IM_DEFINE_ERROR(AcceptError, SERR_ACCEPT, "Accept error")
IM_DEFINE_ERROR(RequestError, SERR_REQUEST, ServerError)
IM_DEFINE_ERROR(ResponseError, SERR_RESPONSE, ServerError)

struct IModLog s_logger = {
    (size_t)-1,
    {
        {SLOG_LISTEN, "[LISTEN]", ANSI_BG_DEFAULT, ANSI_FG_GREEN, ANSI_BG_DEFAULT,
         ANSI_FG_GREEN},
        {SLOG_ACCEPT, "[ACCEPT]", ANSI_BG_DEFAULT, ANSI_FG_GREEN, ANSI_BG_DEFAULT,
         ANSI_FG_GREEN},
        {SLOG_REQUEST, "[REQUEST]", ANSI_BG_DEFAULT, ANSI_FG_YELLOW, ANSI_BG_DEFAULT,
         ANSI_FG_YELLOW},
        {SLOG_RESPONSE, "[RESPONSE]", ANSI_BG_DEFAULT, ANSI_FG_MAGENTA, ANSI_BG_DEFAULT,
         ANSI_FG_MAGENTA},
        {SLOG_CLOSE, "[CLOSE]", ANSI_BG_DEFAULT, ANSI_FG_RED,
         ANSI_BG_DEFAULT, ANSI_FG_RED},
    }};

struct Server {
  char const *ip_address;
  unsigned short port;
  int socket;
  int client_socket;
  struct sockaddr_in socket_address;
  unsigned int socket_address_len;

};

PRIVATE struct ImStr *ErrorMessage(register char const *const prefix) {
  register struct ImStr *str = imnew(ImStr, 0u);
  ImStr_AppendFmt(str, "%s: %s", prefix, strerror(errno));
  return str;
}


PRIVATE void Start(register struct Server *const self) {
  auto int opt = 1;

  imlog(LOG_INFO, "Creating socket");
  NEQ(self->socket = socket(AF_INET, SOCK_STREAM, 0), -1);

  imlog(LOG_INFO, "Setting socket options");
  NEQ(setsockopt(self->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), -1);

  imlog(LOG_INFO, "Binding socket");
  NEQ(bind(self->socket, (struct sockaddr *)&self->socket_address, self->socket_address_len), -1);
}

PRIVATE void __Constructor__(register void *const _self, register struct ImParams *const args) {
  register struct Server *const self = _self;
  
  if (ImParams_Match(args, 2u, PARAM_PTR, PARAM_UNSIGNED_SHORT) == IM_FALSE) {
    impanic("Server constructor takes (char const *, unsigned short)");
  }

  ImParams_Extract(args, &self->ip_address, &self->port);

  self->socket_address_len = sizeof(self->socket_address);

  self->socket_address.sin_family = AF_INET;
  self->socket_address.sin_port = htons(self->port);
  self->socket_address.sin_addr.s_addr = inet_addr(self->ip_address);

  imlog(LOG_INFO, "Starting server");
  Start(self);
}


PRIVATE void Close(register struct Server *const self) {
  close(self->socket);
  close(self->client_socket);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct Server *const self = _self;
  imlog(LOG_INFO, "Closing server");
  Close(self);
}

PUBLIC struct ImResVoid Server_Listen(register struct Server *const self) {
  enum {MAX_QUEUE_LEN = 20};

  if (listen(self->socket, MAX_QUEUE_LEN) < 0) {
    register struct ImStr *const errmsg =
        ErrorMessage("Listen failed");
    register struct ImError *const error =
        imnew(ListenError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResVoid result = ImResVoid_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    imdel(errmsg);
    return result;
  }

  imodlog(&s_logger, SLOG_LISTEN, "Listening on IP: %s, port: %u\n", 
          inet_ntoa(self->socket_address.sin_addr), ntohs(self->socket_address.sin_port));

  while (IM_TRUE) {
    enum {BUFFER_SIZE = 30720};
    register ssize_t bytes_received = 0;
    auto char buffer[BUFFER_SIZE] = {0};
    auto struct sockaddr_in peer_addr = {0};
    auto socklen_t peer_len = sizeof(peer_addr);
    auto char client_ip[INET_ADDRSTRLEN] = {0};
    register unsigned short client_port = 0;

    imlog(LOG_INFO, "Waiting for a new connection");

    self->client_socket = accept(self->socket, (struct sockaddr *)&self->socket_address, &self->socket_address_len);

    if (self->client_socket < 0) {
      register struct ImStr *const errmsg =
          ErrorMessage("Accept failed");
      register struct ImError *const error =
          imnew(AcceptError, 1u, PARAM_PTR, ImStr_View(errmsg));
      register struct ImResVoid result = ImResVoid_Err(error);
      imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

      imdel(errmsg);
      return result;
    }

    /* get peer info for logging */
    if (getpeername(self->client_socket, (struct sockaddr *)&peer_addr, &peer_len) == 0) {
      inet_ntop(AF_INET, &peer_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
      client_port = ntohs(peer_addr.sin_port);
      imodlog(&s_logger, SLOG_ACCEPT, "IP: %s, PORT: %d\n", client_ip, client_port);
    } else {
      imlog1(LOG_WARN, "Could not retrieve client information: %s", strerror(errno));
    }

    bytes_received = read(self->client_socket, buffer, sizeof(buffer));
    if (bytes_received < 0) {
      register struct ImStr *const errmsg =
          ErrorMessage("Read failed");
      register struct ImError *const error =
          imnew(RequestError, 1u, PARAM_PTR, ImStr_View(errmsg));
      register struct ImResVoid result = ImResVoid_Err(error);
      imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

      imdel(errmsg);
      return result;
    }

    imodlog(&s_logger, SLOG_REQUEST, "\n%s\n", buffer);
    imodlog(&s_logger, SLOG_REQUEST, "Parsing the request\n");
    {
      register struct HttpRequest *const request = imnew(HttpRequest, 1u, PARAM_PTR, buffer);
      (void)imdel(request);
    }

    {
      register struct HttpResponse *const response = imnew(HttpResponse, 0u);
      register char const *res = NULL;

      HttpResponse_SetMimeType(response, MIME_TEXT_HTML);
      ImStr_Append(HttpResponse_GetBody(response), "<h1>Imlib is the best</h1>");
      HttpResponse_Finalize(response);
      res = imtostr(response);
      (void)imdel(response);

      imodlog(&s_logger, SLOG_RESPONSE, "\n%s\n", res);
      write(self->client_socket, res, strlen(res));
      (void)imfree((void *)res);
    }

    if (client_ip[0] != '\0') {
      imodlog(&s_logger, SLOG_CLOSE, "IP: %s, PORT: %d\n", client_ip, client_port);
    } else {
      imodlog(&s_logger, SLOG_CLOSE, "Unknown connection");
    }

    close(self->client_socket);
  }

  return ImResVoid_Ok(SERR_OK);
}

CLASS(Server) {
  _Server.size = sizeof(struct Server);
  _Server.ctor = __Constructor__;
  _Server.dtor = __Destructor__;
}


