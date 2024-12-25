#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "imlib/imclass.h"
#include "imlib/imstr.h"

struct HttpRequest;
extern struct ImClass *const HttpRequest;

PUBLIC char const *GetMethod(struct HttpRequest const *self);
PUBLIC char const *GetPath(struct HttpRequest const *self);
PUBLIC int GetMinorVersion(struct HttpRequest const *self);
PUBLIC struct ImOptPtr GetHeaderValueFromKey(struct HttpRequest const *self, struct ImStr const *key);
PUBLIC char const *GetBody(struct HttpRequest const *self);

#endif /* !HTTP_REQUEST_H_ */

