#include "ihandler.h"

#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/immem.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"

PRIVATE void __Constructor__(register void *const self,
                             register struct ImParams *const args) {
  (void)self, (void)args;
}

PRIVATE void __Destructor__(register void *const self) { (void)self; }

PUBLIC struct HttpResponse *
HttpHandler_Handle(register void *const handler,
                   register struct HttpRequest *const request) {
  register struct HttpHandler *const self = imnew(HttpHandler, 0u);
  register struct ImClass *const klass = imclass(handler);
  register struct HttpResponse *response = NULL;
  klass->implof(self);
  response = self->handle(handler, request);
  (void)imdel(self);
  return response;
}

CLASS(HttpHandler) {
  _HttpHandler.size = sizeof(struct HttpHandler);
  _HttpHandler.ctor = __Constructor__;
  _HttpHandler.dtor = __Destructor__;
}
