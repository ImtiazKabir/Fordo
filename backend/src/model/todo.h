#ifndef TODO_H_
#define TODO_H_

#include "imlib/imclass.h"

struct Todo {
  int id;
  char const *text;
  int is_done;
};

extern struct ImClass *const Todo;


#endif /* !TODO_H_ */

