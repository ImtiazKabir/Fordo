#ifndef TODO_H_
#define TODO_H_

#include "imlib/imclass.h"

#include "cJSON/cJSON.h"

struct Todo {
  int id;
  char const *text;
  int is_done;
};

extern struct ImClass *const Todo;

PUBLIC cJSON *Todo_ToJson(struct Todo const *self);

#endif /* !TODO_H_ */

