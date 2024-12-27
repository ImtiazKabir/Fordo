#ifndef API_DELETE_TODO_H_
#define API_DELETE_TODO_H_

#include "imlib/imclass.h"

#include "../fordodb/fordodb.h"

struct DeleteTodoApiHandler {
  struct FordoDB *db;
};

extern struct ImClass *const DeleteTodoApiHandler;

#endif /* !API_DELETE_TODO_H_ */

