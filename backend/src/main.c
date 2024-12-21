#include "fordodb/fordodb.h"

#include "imlib/imerrno.h"
#include "imlib/imlog.h"
#include "imlib/immem.h"
#include "imlib/imoption.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

PRIVATE void Start(void) {
  register struct FordoDB *const db =
      imnew(FordoDB, 1u, PARAM_PTR, "database/fordo.db");

  register int user_id = 0;
  register int todo_id = 0;

  user_id = ImResInt_Unwrap(FordoDB_AddUser(db, "Prova", "firefly"));
  todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "fly"));
  ImResVoid_Unwrap(FordoDB_DeleteTodo(db, todo_id));
  todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "sing"));
  ImResVoid_Unwrap(FordoDB_DeleteTodo(db, todo_id));


  imdel(db);
}

PUBLIC int main(register int const argc,
                register char const *const *const argv) {
  (void)argc;
  trace_target = argv[0];
  imclrerr();
  errno = 0;
  /* imlogsetmsk(0); */

  Start();
  return EXIT_SUCCESS;
}
