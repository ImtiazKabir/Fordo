#ifndef API_GET_TODOS_TODO_H_
#define API_GET_TODOS_TODO_H_

#include "imlib/imclass.h"

#include "../fordodb/fordodb.h"

struct GetTodosApiHandler {
  struct FordoDB *db;
};

extern struct ImClass *const GetTodosApiHandler;

#endif /* !API_GET_TODOS_TODO_H_ */

