#include "imlib/iiter.h"
#include "imlib/imerrno.h"
#include "imlib/imio.h"
#include "imlib/imlog.h"
#include "imlib/immem.h"
#include "imlib/imoption.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"
#include "imlib/list/ilist.h"
#include "imlib/list/linkedlist.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "fordodb/fordodb.h"
#include "model/todo.h"
#include "server/server.h"

PRIVATE void DBDebug(void) {
  register struct FordoDB *const db =
      imnew(FordoDB, 1u, PARAM_PTR, "database/fordo.db");

  {
    register int user_id = 0;
    register int todo_id = 0;

    user_id = ImResInt_Unwrap(FordoDB_AddUser(db, "Imtiaz", "imkabir"));
    todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "fly"));
    ImResVoid_Unwrap(FordoDB_DeleteTodo(db, todo_id));
    todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "die"));
    ImResVoid_Unwrap(FordoDB_ToggleTodo(db, todo_id));
    todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "hapi-hapi-hapi"));
    todo_id = ImResInt_Unwrap(FordoDB_AddTodo(db, user_id, "hibijibi"));
    ImResVoid_Unwrap(FordoDB_ToggleTodo(db, todo_id));
  }

  {
    register int user_id = 0;
    register struct ImLinkedList *todos = NULL;
    register struct ImLLIter *iter = NULL;

    user_id = ImResInt_Unwrap(FordoDB_GetUserId(db, "Imtiaz", "imkabir"));
    todos = ImResPtr_Unwrap(FordoDB_GetAllTodo(db, user_id));

    iter = imnew(ImLLIter, 1u, PARAM_PTR, todos);
    while (IM_TRUE) {
      register struct ImOptPtr const nxt = ImIIter_Next(iter);
      if (ImOptPtr_IsNone(nxt) != IM_FALSE) {
        break;
      } else {
        register struct Todo *const todo = ImOptPtr_Unwrap(nxt);
        imputobj(todo, stdout);
        fputc('\n', stdout);
      }
    }

    imdel(iter);
    imdel(todos);
  }

  imdel(db);
}

PRIVATE void ServerDebug(void) {
  register struct Server *const server =
      imnew(Server, 2u, PARAM_PTR, "0.0.0.0", PARAM_UNSIGNED_SHORT, 3000u);

  ImResVoid_Unwrap(Server_Listen(server));

  imdel(server);
}

PUBLIC int main(register int const argc,
                register char const *const *const argv) {
  (void)argc;
  trace_target = argv[0];
  imclrerr();
  errno = 0;
  /* imlogsetmsk(0); */

  /* DBDebug(); */
  ServerDebug();

  return EXIT_SUCCESS;
}
