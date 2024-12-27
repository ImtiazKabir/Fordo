#ifndef API_COMMON_H_
#define API_COMMON_H_

#include "imlib/imstdinc.h"
#include "imlib/iiter.h"
#include "cJSON/cJSON.h"

#include "../response/response.h"

PUBLIC struct HttpResponse *FordoAPI_GetFailureResponse(void *errors);
PUBLIC struct HttpResponse *FordoAPI_GetSuccessResponse(cJSON *data);
PUBLIC char *FordoAPI_JwtEncodeUserId(int user_id, char const *secret);
PUBLIC int FordoAPI_JwtDecodeUserId(char const *token, char const *secret);

#endif /* API_COMMON_H_ */

