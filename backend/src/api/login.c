#include "login.h"

#include "../handler/ihandler.h"

#include <string.h>
#include <unistd.h>

#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/imlog.h"
#include "imlib/immem.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"
#include "imlib/imstr.h"
#include "imlib/imoption.h"

#include "../http_mimes/http_mimes.h"
#include "../request/request.h"
#include "../response/response.h"
#include "../util/file_util.h"


PRIVATE void __Constructor__(register void *const _self,
                             register struct ImParams *const args) {
  register struct LoginApiHandler *const self = _self;
  if (ImParams_Match(args, 1u, PARAM_PTR) == IM_FALSE) {
    impanic("LoginApiHandler Constructor takes (struct FordoDB *)");
  }
  ImParams_Extract(args, &self->db);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct LoginApiHandler *const self = _self;
  (void)self;
}

PRIVATE struct ImOptPtr
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct FileHttpHandler *const self = _self;
  register struct HttpResponse *response = NULL;
  (void)self;

  if (strcmp(HttpRequest_GetPath(request), "/api/login") != 0) {
    return ImOptPtr_None();
  }

  response = imnew(HttpResponse, 0u);
  HttpResponse_SetMimeType(response, MIME_APPLICATION_JSON);
  ImStr_Append(HttpResponse_GetBody(response), "{\"user_id\": 456}");
  HttpResponse_Finalize(response);
  return ImOptPtr_Some(response);
}

PRIVATE void __InterfaceImplementation__(register void *const interface) {
  if (imisof(interface, HttpHandler) != IM_FALSE) {
    register struct HttpHandler *const handler_interface = interface;
    handler_interface->handle = __Handle__;
  } else {
    impanic("LoginApiHandler does not implement %s\n", imtype(interface));
  }
}

CLASS(LoginApiHandler) {
  _LoginApiHandler.size = sizeof(struct LoginApiHandler);
  _LoginApiHandler.ctor = __Constructor__;
  _LoginApiHandler.dtor = __Destructor__;
  _LoginApiHandler.implof = __InterfaceImplementation__;
}
