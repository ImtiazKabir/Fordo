#ifndef API_ADD_TODO_H_
#define API_ADD_TODO_H_

#include "imlib/imclass.h"

#include "../fordodb/fordodb.h"

struct AddTodoApiHandler {
  struct FordoDB *db;
};

extern struct ImClass *const AddTodoApiHandler;

#endif /* !API_ADD_TODO_H__ */

