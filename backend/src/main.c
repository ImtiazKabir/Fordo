#include "imlib/imerrno.h"
#include "imlib/imlog.h"
#include "imlib/immem.h"
#include "imlib/imoption.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"
#include "imlib/imio.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "fordodb/fordodb.h"
#include "model/todo.h"

PRIVATE void Start(void) {
  register struct FordoDB *const db =
      imnew(FordoDB, 1u, PARAM_PTR, "database/fordo.db");

  register int user_id = 0;
  register int todo_id = 0;

  user_id = ImResInt_Unwrap(FordoDB_AddUser(db, "Imtiaz", "imkabir"));
  todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "fly"));
  ImResVoid_Unwrap(FordoDB_DeleteTodo(db, todo_id));
  todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "die"));
  ImResVoid_Unwrap(FordoDB_ToggleTodo(db, todo_id));

  imdel(db);
}

PRIVATE void Debug(void) {
  register struct Todo *const todo = imnew(Todo, 3u, PARAM_INT, 4, PARAM_PTR, "Hello", PARAM_INT, 1);
  imputobj(todo, stdout);
  imdel(todo);
}

PUBLIC int main(register int const argc,
                register char const *const *const argv) {
  (void)argc;
  trace_target = argv[0];
  imclrerr();
  errno = 0;
  /* imlogsetmsk(0); */

  /* Start(); */
  Debug();
  return EXIT_SUCCESS;
}
