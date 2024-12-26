#include "response.h"

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
#include "imlib/imstr.h"
#include "imlib/iiter.h"
#include "imlib/impair.h"
#include "imlib/imlog.h"

#include "../http_status/http_status.h"

struct HttpResponse {
  enum HttpStatusCode status_code;
  int minor_version;
  void *header_map;
  struct ImStr *body;
};

PRIVATE void __Constructor__(register void *const _self, register struct ImParams *const args) {
  register struct HttpResponse *const self = _self;

  if (ImParams_Match(args, 0u, PARAM_PTR) == IM_FALSE) {
    impanic("HttpResponse constructor takes (void)");
  }

  self->header_map = imnew(ImChainMap, 0u);
  ImIMap_SetKeyPolicy(self->header_map, POLICY_TRANSFER);
  ImIMap_SetValPolicy(self->header_map, POLICY_TRANSFER);

  self->body = imnew(ImStr, 0u);

  self->minor_version = 1;
  self->status_code = HTTP_STATUS_OK;
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct HttpResponse *const self = _self;
  (void)imdel(self->body);
  (void)imdel(self->header_map);
}

PUBLIC void HttpResponse_SetStatusCode(
  register struct HttpResponse *const self, 
  register enum HttpStatusCode const status_code) {
  self->status_code = status_code;
}

PUBLIC void HttpResponse_SetMinorVersion(
  register struct HttpResponse *const self,
  register int const minor_version) {
  self->minor_version = minor_version;
}

PUBLIC void HttpResponse_AddHeader(
  register struct HttpResponse *const self,
  register struct ImStr const *const header_key,
  register struct ImStr *const header_value) {
  ImIMap_AddOrReplace(self->header_map, header_key, header_value);
}

PUBLIC void HttpResponse_AddHeaderCstr(
  register struct HttpResponse *const self,
  register char const *const key,
  register char const *const value
) {
  HttpResponse_AddHeader(self, imnew(ImStr, 1u, PARAM_PTR, key), imnew(ImStr, 1u, PARAM_PTR, value));
}

PUBLIC struct ImStr *HttpResponse_GetBody(register struct HttpResponse *const self) {
  return self->body;
}

PUBLIC void HttpResponse_Finalize(register struct HttpResponse *const self) {
  register struct ImStr *const key = imnew(ImStr, 1u, PARAM_PTR, "Content-Length");
  register struct ImStr *const val = imnew(ImStr, 0u);
  ImStr_AppendFmt(val, "%lu", ImStr_Length(self->body));
  HttpResponse_AddHeader(self, key, val);
}

PUBLIC void HttpResponse_SetMimeType(
  register struct HttpResponse *const self,
  register enum MimeType const mime) {
  register struct ImStr *const key = imnew(ImStr, 1u, PARAM_PTR, "Content-Type");
  register struct ImStr *const val = imnew(ImStr, 0u);
  ImStr_AppendFmt(val, "%s/%s", mime_types[mime], mime_subtypes[mime]);
  HttpResponse_AddHeader(self, key, val);
}


PRIVATE void HeaderAppend(register void *const _pair, register void *const _str) {
  register struct ImPair *const pair = _pair;
  register struct ImStr *const str = _str;
  register struct ImStr *const key = ImPair_KeyView(pair);
  register struct ImStr *const val = ImPair_ValueView(pair);
  
  ImStr_Append(str, ImStr_View(key));
  ImStr_Append(str, ": ");
  ImStr_Append(str, ImStr_View(val));
  ImStr_Append(str, "\r\n");
}

PRIVATE char *ToStr(register void const *const _self) {
  register struct HttpResponse const *const self = _self;
  register char *ret = NULL;
  register struct ImStr *const sb = imnew(ImStr, 0u);

  ImStr_AppendFmt(sb, "HTTP/1.%d", self->minor_version);
  ImStr_AppendFmt(sb, " %d %s", http_status_codes[self->status_code], http_status_messages[self->status_code]);
  ImStr_Append(sb, "\r\n");
  
  {
    register void *const iter = imnew(ImCMIter, 1u, PARAM_PTR, self->header_map);
    ImIIter_ForEach(iter, HeaderAppend, sb);
    (void)imdel(iter);
  }
  
  ImStr_Append(sb, "\r\n");
  ImStr_Append(sb, ImStr_View(self->body));
  ret = imtostr(sb);
  (void)imdel(sb);
  return ret;
}


CLASS(HttpResponse) {
  _HttpResponse.size = sizeof(struct HttpResponse);
  _HttpResponse.ctor = __Constructor__;
  _HttpResponse.dtor = __Destructor__;
  _HttpResponse.tostr = ToStr;
}

