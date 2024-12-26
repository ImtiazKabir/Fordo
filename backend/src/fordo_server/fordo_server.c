#include "fordo_server.h"
#include "../server/server.h"
#include "../server/server_prot.h"

#include "imlib/imstdinc.h"
#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/imparam.h"
#include "imlib/impanic.h"
#include "imlib/list/linkedlist.h"
#include "imlib/list/ilist.h"
#include "imlib/iiter.h"
#include "imlib/imlog.h"

#include "../handler/file_handler.h"

#include "../fordodb/fordodb.h"

struct FordoServer {
  struct Server server;
  void *handler_list;
  void *handler_iter;
  struct FordoDB *db;
};


PRIVATE void __Constructor__(register void *const _self,
                             register struct ImParams *const args) {
  register struct FordoServer *const self = _self;
  (void)args;

  self->handler_list = imnew(ImLinkedList, 0u);
  ImIList_SetPolicy(self->handler_list, POLICY_TRANSFER);

  imlog(LOG_INFO, "Instantiating all handlers");
  ImIList_Append(self->handler_list, imnew(FileHttpHandler, 1u, PARAM_PTR, "public"));

  self->handler_iter = imnew(ImLLIter, 1u, PARAM_PTR, self->handler_list);

  imlog(LOG_INFO, "Registering all handlers");
  Server_SetHandlerChain(&self->server, self->handler_iter);

  self->db = imnew(FordoDB, 1u, PARAM_PTR, "database/fordo.db");
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct FordoServer *const self = _self;
  (void)imdel(self->handler_iter);
  (void)imdel(self->handler_list);

  (void)imdel(self->db);
}

PRIVATE void __SuperParams__(register struct ImParams *sup_args,
                                    register struct ImParams *self_args) {
  auto char const *ip_address = NULL;
  auto unsigned short port = 0u;

  if (ImParams_Match(self_args, 2u, PARAM_PTR, PARAM_UNSIGNED_SHORT) == IM_FALSE) {
    impanic("FordoServer constructor takes (char const *, unsigned short)");
  }

  ImParams_Extract(self_args, &ip_address, &port);
  ImParams_Push(sup_args, 2u, PARAM_PTR, ip_address, PARAM_UNSIGNED_SHORT, port);
}

CLASS(FordoServer) {
  _FordoServer.size = sizeof(struct FordoServer);
  _FordoServer.ctor = __Constructor__;
  _FordoServer.dtor = __Destructor__;
  _FordoServer.super_params = __SuperParams__;
  _FordoServer.super_class = Server;
}
