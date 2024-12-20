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


PRIVATE void Start(void) {
  register struct FordoDB *const fordoDB = imnew(FordoDB, 1u, PARAM_PTR, "database/fordo.db");

  DBResult_Unwrap(FordoDB_AddUser(fordoDB, "Ramanujan", "ramuuuujan"));
  DBResult_Unwrap(FordoDB_AddTodo(fordoDB, 1, "Go do math"));

  imdel(fordoDB);
}


PUBLIC int main(register int const argc, register char const *const *const argv) {
  (void)argc;
  trace_target = argv[0];
  imclrerr();
  errno = 0;
  /* imlogsetmsk(0); */

  Start();
  return EXIT_SUCCESS;
}

