#ifndef SERVER_H_
#define SERVER_H_

#include "imlib/imstdinc.h"
#include "imlib/imclass.h"
#include "imlib/imodlog.h"
#include "imlib/imerror.h"

struct Server;
extern struct ImClass *const Server;

enum ServerLogLevel { SLOG_LISTEN, SLOG_ACCEPT, SLOG_REQUEST, SLOG_RESPONSE, SLOG_CLOSE };
extern struct IModLog s_logger;

enum ServerErrorStatus {
  SERR_OK = 0,
  SERR_ERR = -1,
  SERR_LISTEN = -2,
  SERR_ACCEPT = -3,
  SERR_REQUEST = -4,
  SERR_RESPONSE = -5
};

IM_DECLARE_ERROR(ServerError, ImError)
IM_DECLARE_ERROR(ListenError, ServerError)
IM_DECLARE_ERROR(AcceptError, ServerError)
IM_DECLARE_ERROR(RequestError, ServerError)
IM_DECLARE_ERROR(ResponseError, ServerError)

PUBLIC void Server_SetHandlerChain(struct Server *self, void *chain);
PUBLIC void *Server_GetHandlerChain(struct Server *self);
PUBLIC struct ImResVoid Server_Listen(struct Server *self);

#endif /* !SERVER_H_ */

