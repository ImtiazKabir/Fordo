#ifndef API_TOGGLE_TODO_H_
#define API_TOGGLE_TODO_H_

#include "imlib/imclass.h"

#include "../fordodb/fordodb.h"

struct ToggleTodoApiHandler {
  struct FordoDB *db;
};

extern struct ImClass *const ToggleTodoApiHandler;

#endif /* !API_TOGGLE_TODO_H_ */

