#include "add_todo.h"

#include "../handler/ihandler.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/imlog.h"
#include "imlib/immem.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/imstdinc.h"
#include "imlib/imstr.h"
#include "imlib/imoption.h"
#include "imlib/list/ilist.h"
#include "imlib/list/linkedlist.h"
#include "imlib/iiter.h"
#include "imlib/imerror.h"
#include "cJSON/cJSON.h"

#include "../http_mimes/http_mimes.h"
#include "../request/request.h"
#include "../response/response.h"
#include "../util/file_util.h"
#include "common.h"

PRIVATE char *__dupstr__(register char const *const src) {
  return strcpy(imalloct("String", (strlen(src) + 1u) * sizeof(char)), src);
}


PRIVATE void __Constructor__(register void *const _self,
                             register struct ImParams *const args) {
  register struct AddTodoApiHandler *const self = _self;
  if (ImParams_Match(args, 1u, PARAM_PTR) == IM_FALSE) {
    impanic("AddTodoApiHandler Constructor takes (struct FordoDB *)");
  }
  ImParams_Extract(args, &self->db);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct AddTodoApiHandler *const self = _self;
  (void)self;
}

PRIVATE void ParseTodoText(register char const *const json_string,
                             register char const **const todo_text,
                             register void *const error_list) {
  register cJSON *json = NULL;
  register cJSON *todo_text_item = NULL;

  json = cJSON_Parse(json_string);
  if (json == NULL) {
    register struct ImStr *const str = imnew(ImStr, 0u);
    ImStr_Append(str, "Syntax error in json");
    ImIList_Append(error_list, str);
  }

  todo_text_item = cJSON_GetObjectItem(json, "todo_text");
  if (cJSON_IsString(todo_text_item)) {
    *todo_text = __dupstr__(todo_text_item->valuestring);
  } else {
    register struct ImStr *const str = imnew(ImStr, 0u);
    ImStr_Append(str, "'todo_text' field is missing or not a string\n");
    ImIList_Append(error_list, str);
    *todo_text = NULL;
  }

  cJSON_Delete(json);
}

PRIVATE struct ImOptPtr
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct AddTodoApiHandler *const self = _self;
  register struct HttpResponse *response = NULL;
  register void *error_list = NULL;
  register int user_id = 0;
  auto char const *todo_text = NULL;


  if (strcmp(HttpRequest_GetPath(request), "/api/add-todo") != 0) {
    return ImOptPtr_None();
  }

  error_list = imnew(ImLinkedList, 0u);
  ImIList_SetPolicy(error_list, POLICY_TRANSFER);

  user_id = -1;
  {
    register struct ImStr *const key = imnew(ImStr, 1u, PARAM_PTR, "Authorization");
    register struct ImOptPtr const opt = HttpRequest_GetHeaderValueFromKey(request, key);
    if (ImOptPtr_IsNone(opt) != IM_FALSE) {
      ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "No authorization information provided"));
    } else {
      register struct ImStr *const val = ImOptPtr_Unwrap(opt);
      if (strncmp(ImStr_View(val), "Bearer ", 7u) != 0) {
        ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Needed bearer auth"));
      } else {
        register char const *const auth = ImStr_View(val) + 7u; /* Offset for Bearer<SPACE> */
        user_id = FordoAPI_JwtDecodeUserId(auth, getenv("JWT_SECRET"));
        if (user_id < 0) {
          ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Wrong authorization token"));
        }
      }
    }
    (void)imdel(key);
  }

  if (user_id >= 0) {
    ParseTodoText(HttpRequest_GetBody(request), &todo_text, error_list);
    if (todo_text != NULL) {
      register struct ImResInt const result = FordoDB_AddTodo(self->db, user_id, todo_text);
      if (ImResInt_IsErr(result) != IM_FALSE) {
        register struct ImError *const err = ImResInt_UnwrapErr(result);
        if (imisof(err, ExecuteError)) {
          ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Invalid user"));
        } else {
          ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Internal database error"));
        }
        (void)imdel(err);
      } else {
        register int const todo_id = ImResInt_Unwrap(result);
        register cJSON *const data = cJSON_CreateObject();
        cJSON_AddNumberToObject(data, "todo_id", todo_id);
        response = FordoAPI_GetSuccessResponse(data);
      }
    }
  }

  if (ImIList_Len(error_list) > 0) {
    register void *error_iter = NULL;
    error_iter = imnew(ImLLIter, 1u, PARAM_PTR, error_list);
    response = FordoAPI_GetFailureResponse(error_iter);
    (void)imdel(error_iter);
  }

  (void)imfree((void *)todo_text);
  (void)imdel(error_list);
  return ImOptPtr_Some(response);
}

PRIVATE void __InterfaceImplementation__(register void *const interface) {
  if (imisof(interface, HttpHandler) != IM_FALSE) {
    register struct HttpHandler *const handler_interface = interface;
    handler_interface->handle = __Handle__;
  } else {
    impanic("AddTodoApiHandler does not implement %s\n", imtype(interface));
  }
}

CLASS(AddTodoApiHandler) {
  _AddTodoApiHandler.size = sizeof(struct AddTodoApiHandler);
  _AddTodoApiHandler.ctor = __Constructor__;
  _AddTodoApiHandler.dtor = __Destructor__;
  _AddTodoApiHandler.implof = __InterfaceImplementation__;
}
