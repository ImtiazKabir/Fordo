#include "login.h"

#include "../handler/ihandler.h"

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

#include "../http_mimes/http_mimes.h"
#include "../request/request.h"
#include "../response/response.h"
#include "../util/file_util.h"
#include "cJSON/cJSON.h"

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
                             register char const **const password) {
  register cJSON *json = NULL;
  register cJSON *username_item = NULL;
  register cJSON *password_item = NULL;

  json = cJSON_Parse(json_string);
  if (!json) {
    fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
    return;
  }

  username_item = cJSON_GetObjectItem(json, "username");
  if (cJSON_IsString(username_item)) {
    *username = __dupstr__(username_item->valuestring);
  } else {
    imlogf(LOG_ERROR, stderr, "'username' field is missing or not a string\n");
    *username = NULL;
  }

  password_item = cJSON_GetObjectItem(json, "password");
  if (cJSON_IsString(password_item)) {
    *password = __dupstr__(password_item->valuestring);
  } else {
    imlogf(LOG_ERROR, stderr, "'password' field is missing or not a string");
    *password = NULL;
  }

  cJSON_Delete(json);
  return;
}


PRIVATE struct ImOptPtr
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct FileHttpHandler *const self = _self;
  register struct HttpResponse *response = NULL;
  auto char const *username = NULL;
  auto char const *password = NULL;

  (void)self;

  if (strcmp(HttpRequest_GetPath(request), "/api/login") != 0) {
    return ImOptPtr_None();
  }

  ParseCredential(HttpRequest_GetBody(request), &username, &password);
  imlog2(LOG_INFO, "Username: %s, Password: %s", username, password);
  (void)imfree((void *)username);
  (void)imfree((void *)password);

  response = imnew(HttpResponse, 0u);
  HttpResponse_SetMimeType(response, MIME_APPLICATION_JSON);
  ImStr_Append(HttpResponse_GetBody(response), "{\"user_id\": 456}");
  HttpResponse_Finalize(response);
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
