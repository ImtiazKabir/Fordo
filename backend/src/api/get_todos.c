#include "get_todos.h"

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
#include "../model/todo.h"
#include "common.h"

PRIVATE char *__dupstr__(register char const *const src) {
  return strcpy(imalloct("String", (strlen(src) + 1u) * sizeof(char)), src);
}


PRIVATE void __Constructor__(register void *const _self,
                             register struct ImParams *const args) {
  register struct GetTodosApiHandler *const self = _self;
  if (ImParams_Match(args, 1u, PARAM_PTR) == IM_FALSE) {
    impanic("GetTodosApiHandler Constructor takes (struct FordoDB *)");
  }
  ImParams_Extract(args, &self->db);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct GetTodosApiHandler *const self = _self;
  (void)self;
}

PRIVATE struct ImOptPtr
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct GetTodosApiHandler *const self = _self;
  register struct HttpResponse *response = NULL;
  register void *error_list = NULL;
  register int user_id = 0;

  if (strcmp(HttpRequest_GetPath(request), "/api/get-todos") != 0) {
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
    register struct ImResPtr const result = FordoDB_GetAllTodo(self->db, user_id);
    if (ImResPtr_IsErr(result) != IM_FALSE) {
      register struct ImError *const err = ImResPtr_UnwrapErr(result);
      ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Internal server error"));
      (void)imdel(err);
    } else {
      register void *const todo_list = ImResPtr_Unwrap(result);
      register void *const iter = imnew(ImLLIter, 1u, PARAM_PTR, todo_list);
      register cJSON *const data = cJSON_CreateObject();
      register cJSON *const arr = cJSON_CreateArray();
      
      while (IM_TRUE) {
        register struct ImOptPtr const nxt = ImIIter_Next(iter);
        if (ImOptPtr_IsNone(nxt) != IM_FALSE) {
          break;
        } else {
          register struct Todo *const todo = ImOptPtr_Unwrap(nxt);
          cJSON_AddItemToArray(arr, Todo_ToJson(todo));
        }
      }
      cJSON_AddItemToObject(data, "todos", arr);
      response = FordoAPI_GetSuccessResponse(data);
      (void)imdel(iter);
      (void)imdel(todo_list);
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
    impanic("GetTodosApiHandler does not implement %s\n", imtype(interface));
  }
}

CLASS(GetTodosApiHandler) {
  _GetTodosApiHandler.size = sizeof(struct GetTodosApiHandler);
  _GetTodosApiHandler.ctor = __Constructor__;
  _GetTodosApiHandler.dtor = __Destructor__;
  _GetTodosApiHandler.implof = __InterfaceImplementation__;
}

