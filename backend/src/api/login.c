#include "login.h"

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
  register struct LoginApiHandler *const self = _self;
  if (ImParams_Match(args, 1u, PARAM_PTR) == IM_FALSE) {
    impanic("LoginApiHandler Constructor takes (struct FordoDB *)");
  }
  ImParams_Extract(args, &self->db);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct LoginApiHandler *const self = _self;
  (void)self;
}

PRIVATE void ParseCredential(register char const *const json_string,
                             register char const **const username,
                             register char const **const password,
                             register void *const error_list) {
  register cJSON *json = NULL;
  register cJSON *username_item = NULL;
  register cJSON *password_item = NULL;

  json = cJSON_Parse(json_string);
  if (json == NULL) {
    register struct ImStr *const str = imnew(ImStr, 0u);
    ImStr_Append(str, "Syntax error in json");
    ImIList_Append(error_list, str);
  }

  username_item = cJSON_GetObjectItem(json, "username");
  if (cJSON_IsString(username_item)) {
    *username = __dupstr__(username_item->valuestring);
  } else {
    register struct ImStr *const str = imnew(ImStr, 0u);
    ImStr_Append(str, "'username' field is missing or not a string\n");
    ImIList_Append(error_list, str);
    *username = NULL;
  }

  password_item = cJSON_GetObjectItem(json, "password");
  if (cJSON_IsString(password_item)) {
    *password = __dupstr__(password_item->valuestring);
  } else {
    register struct ImStr *const str = imnew(ImStr, 0u);
    ImStr_Append(str, "'password' field is missing or not a string\n");
    ImIList_Append(error_list, str);
    *password = NULL;
  }

  cJSON_Delete(json);
}

PRIVATE struct ImOptPtr
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct LoginApiHandler *const self = _self;
  register struct HttpResponse *response = NULL;
  register void *error_list = NULL;
  auto char const *username = NULL;
  auto char const *password = NULL;

  if (strcmp(HttpRequest_GetPath(request), "/api/login") != 0) {
    return ImOptPtr_None();
  }

  error_list = imnew(ImLinkedList, 0u);
  ImIList_SetPolicy(error_list, POLICY_TRANSFER);

  ParseCredential(HttpRequest_GetBody(request), &username, &password, error_list);

  if (username != NULL && password != NULL) {
    register struct ImResInt const result = FordoDB_GetUserId(self->db, username, password);
    if (ImResInt_IsErr(result) != IM_FALSE) {
      register struct ImError *const err = ImResInt_UnwrapErr(result);
      if (imisof(err, ExecuteError)) {
        ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Invalid credential"));
      } else {
        ImIList_Append(error_list, imnew(ImStr, 1u, PARAM_PTR, "Internal database error"));
      }
      (void)imdel(err);
    } else {
      register int const user_id = ImResInt_Unwrap(result);
      register char const* const user_token = FordoAPI_JwtEncodeUserId(user_id, getenv("JWT_SECRET"));
      register cJSON *const data = cJSON_CreateObject();
      NEQ(user_token, NULL);
      cJSON_AddStringToObject(data, "user_token", user_token);
      response = FordoAPI_GetSuccessResponse(data);
      free((void *)user_token);
    }
  }

  if (ImIList_Len(error_list) > 0) {
    register void *error_iter = NULL;
    error_iter = imnew(ImLLIter, 1u, PARAM_PTR, error_list);
    response = FordoAPI_GetFailureResponse(error_iter);
    (void)imdel(error_iter);
  }

  (void)imfree((void *)username);
  (void)imfree((void *)password);
  (void)imdel(error_list);
  return ImOptPtr_Some(response);
}

PRIVATE void __InterfaceImplementation__(register void *const interface) {
  if (imisof(interface, HttpHandler) != IM_FALSE) {
    register struct HttpHandler *const handler_interface = interface;
    handler_interface->handle = __Handle__;
  } else {
    impanic("LoginApiHandler does not implement %s\n", imtype(interface));
  }
}

CLASS(LoginApiHandler) {
  _LoginApiHandler.size = sizeof(struct LoginApiHandler);
  _LoginApiHandler.ctor = __Constructor__;
  _LoginApiHandler.dtor = __Destructor__;
  _LoginApiHandler.implof = __InterfaceImplementation__;
}
