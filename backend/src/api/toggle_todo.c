#include "toggle_todo.h"

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
  register struct ToggleTodoApiHandler *const self = _self;
  if (ImParams_Match(args, 1u, PARAM_PTR) == IM_FALSE) {
    impanic("ToggleTodoApiHandler Constructor takes (struct FordoDB *)");
  }
  ImParams_Extract(args, &self->db);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct ToggleTodoApiHandler *const self = _self;
  (void)self;
}

PRIVATE void ParseTodoId(register char const *const json_string,
                             register int *const todo_id,
                             register void *const error_list) {
  register cJSON *json = NULL;
  register cJSON *todo_id_item = NULL;

  json = cJSON_Parse(json_string);
  if (json == NULL) {
    register struct ImStr *const str = imnew(ImStr, 0u);
    ImStr_Append(str, "Syntax error in json");
    ImIList_Append(error_list, str);
  }

  todo_id_item = cJSON_GetObjectItem(json, "todo_id");
  if (cJSON_IsNumber(todo_id_item)) {
    *todo_id = todo_id_item->valueint;
  } else {
    register struct ImStr *const str = imnew(ImStr, 0u);
    ImStr_Append(str, "'todo_id' field is missing or not a number\n");
    ImIList_Append(error_list, str);
    *todo_id = -1;
  }

  cJSON_Delete(json);
}

PRIVATE struct ImOptPtr
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct ToggleTodoApiHandler *const self = _self;
  register struct HttpResponse *response = NULL;
  register void *error_list = NULL;
  register int user_id = 0;
  auto int todo_id = 0;


  if (strcmp(HttpRequest_GetPath(request), "/api/toggle-todo") != 0) {
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

  todo_id = -1;
  if (user_id >= 0) {
    ParseTodoId(HttpRequest_GetBody(request), &todo_id, error_list);
    if (todo_id >= 0) {
      register struct ImResInt const owner_result = FordoDB_GetUidOfTodo(self->db, todo_id);
      register int owner_uid = 0;
      if (ImResInt_IsErr(owner_result) != IM_FALSE) {
        register struct ImError *const err = ImResInt_UnwrapErr(owner_result);
        ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Invalid todo id"));
        (void)imdel(err);
      } else {
        owner_uid = ImResInt_Unwrap(owner_result);
        if (owner_uid != user_id) {
          ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "You do not own this todo"));
        } else {
          register struct ImResVoid const result = FordoDB_ToggleTodo(self->db, todo_id);
          if (ImResVoid_IsErr(result) != IM_FALSE) {
            register struct ImError *const err = ImResVoid_UnwrapErr(result);
            ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Internal database error"));
            (void)imdel(err);
          } else {
            response = FordoAPI_GetSuccessResponse(NULL);
          }
        }
      }
    }
  }

  if (ImIList_Len(error_list) > 0) {
    register void *error_iter = NULL;
    error_iter = imnew(ImLLIter, 1u, PARAM_PTR, error_list);
    response = FordoAPI_GetFailureResponse(error_iter);
    (void)imdel(error_iter);
  }

  (void)imdel(error_list);
  return ImOptPtr_Some(response);
}

PRIVATE void __InterfaceImplementation__(register void *const interface) {
  if (imisof(interface, HttpHandler) != IM_FALSE) {
    register struct HttpHandler *const handler_interface = interface;
    handler_interface->handle = __Handle__;
  } else {
    impanic("ToggleTodoApiHandler does not implement %s\n", imtype(interface));
  }
}

CLASS(ToggleTodoApiHandler) {
  _ToggleTodoApiHandler.size = sizeof(struct ToggleTodoApiHandler);
  _ToggleTodoApiHandler.ctor = __Constructor__;
  _ToggleTodoApiHandler.dtor = __Destructor__;
  _ToggleTodoApiHandler.implof = __InterfaceImplementation__;
}

