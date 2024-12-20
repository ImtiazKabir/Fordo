#include "fordodb/fordodb.h"

#include "imlib/imlog.h"
#include "imlib/impanic.h"
#include "imlib/imstdinc.h"
#include "imlib/imerrno.h"
#include "imlib/imoption.h"
#include "imlib/imparam.h"
#include "imlib/immem.h"

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


PRIVATE void start(void) {
  register struct FordoDB *const fordoDB = imnew(FordoDB, 1u, PARAM_PTR, "database/fordo.db");
  FordoDB_AddUserToDB(fordoDB, "Irtiaz", "2005070");
  imdel(fordoDB);
}


PUBLIC int main(register int const argc, register char const *const *const argv) {
  (void)argc;
  trace_target = argv[0];
  imclrerr();
  errno = 0;
  start();
  return EXIT_SUCCESS;
}

