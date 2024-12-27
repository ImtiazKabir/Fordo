#include "todo.h"

#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/imstdinc.h"
#include "imlib/imparam.h"
#include "imlib/impanic.h"
#include "imlib/immem.h"
#include "imlib/imstr.h"

#include "cJSON/cJSON.h"

#include <string.h>

PRIVATE char *__dupstr__(register char const *const src) {
  return strcpy(imalloct("String", (strlen(src) + 1u) * sizeof(char)), src);
}

PRIVATE void __Constructor__(register void *const _self, register struct ImParams *const args) {
  register struct Todo *const self = _self;

  if (ImParams_Match(args, 3u, PARAM_INT, PARAM_PTR, PARAM_INT) == IM_FALSE) {
    impanic("Todo constructor takes (int, char const *, int)");
  }
  
  ImParams_Extract(args, &self->id, &self->text, &self->is_done);

  self->text = __dupstr__(self->text);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct Todo *const self = _self;

  if (self->text != NULL) {
    imfree((void *)self->text);
  }
}

PRIVATE char *__tostr__(register void const *const _self) {
  register struct Todo const *const self = _self;
  register struct ImStr *const str = imnew(ImStr, 0u);
  register char *ret = NULL;

  ImStr_AppendFmt(str, "Todo#%d", self->id);
  if (self->is_done == 0) {
    ImStr_Append(str, " |[Not done]|");
  } else {
    ImStr_Append(str, "|[Done]|");
  }
  ImStr_Append(str, self->text);

  ret = imtostr(str);
  imdel(str);

  return ret;
}

PUBLIC cJSON *ToJson(register struct Todo const *const self) {
  register cJSON *const json = cJSON_CreateObject();
  cJSON_AddNumberToObject(json, "id", self->id);
  cJSON_AddStringToObject(json, "todo_text", self->text);
  cJSON_AddBoolToObject(json, "is_done", self->is_done);
  return json;
}

CLASS(Todo) {
  _Todo.size = sizeof(struct Todo);
  _Todo.ctor = __Constructor__;
  _Todo.dtor = __Destructor__;
  _Todo.tostr = __tostr__;
}


