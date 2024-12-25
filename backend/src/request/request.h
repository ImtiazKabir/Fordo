#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "imlib/imclass.h"

struct HttpRequest {
  char const *method;
  char const *path;
  int minor_version;
  void *header_map;
  char const *body;
};

extern struct ImClass *const HttpRequest;

#endif /* !HTTP_REQUEST_H_ */

