#ifndef IHANDLER_H_
#define IHANDLER_H_

#include "imlib/imclass.h"
#include "imlib/imstdinc.h"
#include "imlib/imoption.h"

#include "../request/request.h"
#include "../response/response.h"

struct HttpHandler {
  struct ImOptPtr (*handle)(void *handler, struct HttpRequest *request);
};

extern struct ImClass *const HttpHandler;

PUBLIC struct ImOptPtr HttpHandler_Handle(void *handler,
                                               struct HttpRequest *request);

#endif /* !IHANDLER_H_ */
