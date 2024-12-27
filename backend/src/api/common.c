#include "common.h"

#include <stdlib.h>
#include <string.h>

#include "imlib/imstdinc.h"
#include "imlib/iiter.h"
#include "imlib/imstr.h"
#include "imlib/impanic.h"
#include "imlib/imoption.h"
#include "imlib/imlog.h"

#include "cJSON/cJSON.h"
#include "jwt.h"

#include "../response/response.h"


PRIVATE char const *GetErrorBody(register void *const errors) {
  register cJSON *root = NULL;
  register cJSON *messageArray = NULL;
  register char const *ret = NULL;

  NEQ(root = cJSON_CreateObject(), NULL);
  NEQ(cJSON_AddStringToObject(root, "status", "error"), NULL);
  NEQ(cJSON_AddNullToObject(root, "data"), NULL);

  NEQ(messageArray = cJSON_CreateArray(), NULL);
  while(IM_TRUE) {
    register struct ImOptPtr const nxt = ImIIter_Next(errors);
    register struct ImStr const *str = NULL;
    register char const *errstr = NULL;
    if (ImOptPtr_IsNone(nxt) != IM_FALSE) {
      break;
    }
    str = ImOptPtr_Unwrap(nxt);
    errstr = ImStr_View(str);
    NEQ(cJSON_AddItemToArray(messageArray, cJSON_CreateString(errstr)), 0);
  }

  NEQ(cJSON_AddItemToObject(root, "message", messageArray), 0);

  ret = cJSON_Print(root);
  cJSON_Delete(root);
  return ret;
}

PUBLIC struct HttpResponse *FordoAPI_GetFailureResponse(register void *const errors) {
  register struct HttpResponse *const response = imnew(HttpResponse, 0u);
  register char const *const error_body = GetErrorBody(errors);
  HttpResponse_SetStatusCode(response, HTTP_STATUS_BAD_REQUEST);
  HttpResponse_SetMimeType(response, MIME_APPLICATION_JSON);
  ImStr_Append(HttpResponse_GetBody(response), error_body);
  HttpResponse_Finalize(response);
  if (error_body != NULL) {
    free((void *)error_body);
  }
  return response;
}

PRIVATE char const *GetSuccessBody(register cJSON *const data) {
  register cJSON *root = NULL;
  register char const *ret = NULL;

  NEQ(root = cJSON_CreateObject(), NULL);
  NEQ(cJSON_AddStringToObject(root, "status", "success"), NULL);

  if (data != NULL) {
    NEQ(cJSON_AddItemToObject(root, "data", data), 0);
  } else {
    NEQ(cJSON_AddNullToObject(root, "data"), 0);
  }

  NEQ(cJSON_AddNullToObject(root, "message"), 0);

  ret = cJSON_Print(root);
  cJSON_Delete(root);
  return ret;
}

PUBLIC struct HttpResponse *FordoAPI_GetSuccessResponse(register cJSON *const data) {
  register char const *const success_body = GetSuccessBody(data);
  register struct HttpResponse *const response = imnew(HttpResponse, 0u);
  ImStr_Append(HttpResponse_GetBody(response), success_body);
  if (success_body != NULL) {
    free((void *)success_body);
  }
  HttpResponse_Finalize(response);
  return response;
}

PUBLIC char *FordoAPI_JwtEncodeUserId(int user_id, char const *secret) {
  jwt_t *jwt = NULL;
  char *token = NULL;

  EQ(jwt_new(&jwt), 0);
  EQ(jwt_add_grant_int(jwt, "user_id", user_id), 0);
  EQ(jwt_set_alg(jwt, JWT_ALG_HS256, (unsigned char *)secret, (int)strlen(secret)), 0);
  NEQ(token = jwt_encode_str(jwt), NULL);

  jwt_free(jwt);
  imlog2(LOG_INFO, "Encoded user_id %d to user_token %s", user_id, token);
  return token;
}

PUBLIC int FordoAPI_JwtDecodeUserId(char const *token, char const *secret) {
  jwt_t *jwt = NULL;
  int user_id = 0;

  if (jwt_decode(&jwt, token, (unsigned char *)secret, (int)strlen(secret)) != 0) {
    imlogf(LOG_ERROR, stderr, "Invalid jwt");
    return -1;
  }

  user_id = (int)jwt_get_grant_int(jwt, "user_id");
  
  jwt_free(jwt);
  imlog2(LOG_INFO, "Decoded user_token %s to user_id %d", token, user_id);
  return user_id;
}

