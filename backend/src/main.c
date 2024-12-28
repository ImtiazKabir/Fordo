#include "imlib/imerrno.h"
#include "imlib/imoption.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"
#include "imlib/list/ilist.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "server/server.h"
#include "fordo_server/fordo_server.h"

static struct Server *server = NULL;

PRIVATE void ServerRun(void) {
  server = imnew(FordoServer, 2u, PARAM_PTR, "0.0.0.0", PARAM_UNSIGNED_SHORT, 3000u);

  ImResVoid_Unwrap(Server_Listen(server));

  (void)imdel(server);
}

PRIVATE void InterruptHandler(register int const signal_num) { 
  (void)signal_num;
  (void)imdel(server);
  exit(EXIT_SUCCESS);
} 

PUBLIC int main(register int const argc,
                register char const *const *const argv) {
  (void)argc;
  trace_target = argv[0];
  imclrerr();
  errno = 0;

  signal(SIGINT, InterruptHandler); 

  /* imlogsetmsk(0); */

  ServerRun();

  return EXIT_SUCCESS;
}
