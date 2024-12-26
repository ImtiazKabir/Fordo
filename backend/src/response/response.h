#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include "imlib/imclass.h"
#include "imlib/imstr.h"

#include "../http_status/http_status.h"
#include "../http_mimes/http_mimes.h"

struct HttpResponse;
extern struct ImClass *const HttpResponse;

PUBLIC void HttpResponse_SetStatusCode(struct HttpResponse *self, enum HttpStatusCode status_code);
PUBLIC void HttpResponse_SetMinorVersion(struct HttpResponse *self, int minor_version);
PUBLIC void HttpResponse_AddHeader(struct HttpResponse *self, struct ImStr const *header_key, struct ImStr *header_value);
PUBLIC void HttpResponse_AddHeaderCstr(struct HttpResponse *self, char const *key, char const *value);
PUBLIC struct ImStr *HttpResponse_GetBody(struct HttpResponse *self);
PUBLIC void HttpResponse_SetMimeType(struct HttpResponse *self, enum MimeType mime);
PUBLIC void HttpResponse_Finalize(struct HttpResponse *self);

#endif /* !HTTP_RESPONSE_H_ */

