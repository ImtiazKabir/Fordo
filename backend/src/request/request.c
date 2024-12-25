#include "request.h"

#include <string.h>

#include "imlib/imstdinc.h"
#include "imlib/immem.h"
#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/impanic.h"
#include "imlib/imparam.h"
#include "imlib/map/imap.h"
#include "imlib/imstr.h"
#include "imlib/map/imap.h"
#include "imlib/map/chainmap.h"
#include "imlib/imoption.h"

#include "picohttpparser.h"


struct HttpRequest {
  char const *method;
  char const *path;
  int minor_version;
  void *header_map;
  char const *body;
};

PRIVATE char const *GetCString(register char const *const str,
                               register size_t const len) {
  register char *const cstr = imalloct("String", (len + 1u) * sizeof(*cstr));
  FOR(size_t i, i = 0u; i < len; i+= 1u, {
    cstr[i] = str[i];
  })
  cstr[len] = '\0';
  return cstr;
}

PRIVATE char const *__dupstr__(register char const *const src) {
  return strcpy(imalloct("String", (strlen(src) + 1u) * sizeof(char)), src);
}


PRIVATE void Init(register struct HttpRequest *const self,
                  register char const *const request) {
  const char *method, *path;
  int pret, minor_version;
  struct phr_header headers[16];
  size_t method_len, path_len, num_headers;
  const char *body;

  num_headers = sizeof(headers) / sizeof(headers[0]);

  pret = phr_parse_request(request, strlen(request),
                           &method, &method_len,
                           &path, &path_len,
                           &minor_version, headers,
                           &num_headers, 0);

  if (pret == -1) {
    impanic("Failed to parse HTTP request\n");
  }

  self->method = GetCString(method, method_len);
  self->path = GetCString(path, path_len);
  self->minor_version = minor_version;

  self->header_map = imnew(ImChainMap, 0u);
  ImIMap_SetKeyPolicy(self->header_map, POLICY_TRANSFER);
  ImIMap_SetValPolicy(self->header_map, POLICY_TRANSFER);
  FOR(size_t i, i = 0; i < num_headers; i += 1u, {
    register struct ImStr *const key = imnew(ImStr, 0u);
    register struct ImStr *const value = imnew(ImStr, 0u);
    
    ImStr_AppendFmt(key, "%.*s", headers[i].name_len, headers[i].name);
    ImStr_AppendFmt(value, "%.*s", headers[i].value_len, headers[i].value);


    ImIMap_AddOrReplace(self->header_map, key, value);
  })

  body = request + pret;
  self->body = __dupstr__(body);
}


PRIVATE void __Constructor__(register void *const _self, register struct ImParams *const args) {
  register struct HttpRequest *const self = _self;
  auto char const *request = NULL;

  if (ImParams_Match(args, 1u, PARAM_PTR) == IM_FALSE) {
    impanic("HttpRequest constructor takes (char *)");
  }

  ImParams_Extract(args, &request);
  Init(self, request);
}

PRIVATE void Deinit(register struct HttpRequest *const self) {
  (void)imfree((void *)self->method);
  self->method = NULL;

  (void)imfree((void *)self->path);
  self->path = NULL;

  (void)imdel((void *)self->header_map);
  self->header_map = NULL;

  (void)imfree((void *)self->body);
  self->body = NULL;
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct HttpRequest *const self = _self;
  Deinit(self);
}

PUBLIC char const *GetMethod(register struct HttpRequest const *const self) {
  return self->method;
}

PUBLIC char const *GetPath(register struct HttpRequest const *const self) {
  return self->path;
}

PUBLIC int GetMinorVersion(register struct HttpRequest const *const self) {
  return self->minor_version;
}

PUBLIC struct ImOptPtr GetHeaderValueFromKey(
  register struct HttpRequest const *const self,
  register struct ImStr const *const key) {
  return ImIMap_Get(self->header_map, key);
}

PUBLIC char const *GetBody(register struct HttpRequest const *const self) {
  return self->body;
}

CLASS(HttpRequest) {
  _HttpRequest.size = sizeof(struct HttpRequest);
  _HttpRequest.ctor = __Constructor__;
  _HttpRequest.dtor = __Destructor__;
}


