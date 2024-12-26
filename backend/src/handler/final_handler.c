#include "final_handler.h"

#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/immem.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"

#include "../http_mimes/http_mimes.h"
#include "ihandler.h"

PRIVATE void __Constructor__(register void *const self,
                             register struct ImParams *const args) {
  (void)self, (void)args;
  if (ImParams_Match(args, 0u) == IM_FALSE) {
    impanic("FinalHandler Constructor takes (void)");
  }
}

PRIVATE void __Destructor__(register void *const self) { (void)self; }

PRIVATE struct HttpResponse *
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct FinalHttpHandler *const self = _self;
  register struct HttpResponse *const response = imnew(HttpResponse, 0u);

  (void)self, (void)request;

  HttpResponse_SetMimeType(response, MIME_TEXT_HTML);
  ImStr_Append(HttpResponse_GetBody(response), "<h1>From controller</h1>");
  HttpResponse_Finalize(response);
  return response;
}

PRIVATE void __InterfaceImplementation__(register void *const interface) {
  if (imisof(interface, HttpHandler) != IM_FALSE) {
    register struct HttpHandler *const handler_interface = interface;
    handler_interface->handle = __Handle__;
  } else {
    impanic("FinalHttpHandler does not implement %s\n", imtype(interface));
  }
}

CLASS(FinalHttpHandler) {
  _FinalHttpHandler.size = sizeof(struct FinalHttpHandler);
  _FinalHttpHandler.ctor = __Constructor__;
  _FinalHttpHandler.dtor = __Destructor__;
  _FinalHttpHandler.implof = __InterfaceImplementation__;
}
