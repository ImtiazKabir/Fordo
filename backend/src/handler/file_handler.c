#include "file_handler.h"

#include "ihandler.h"

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

PRIVATE char *__dupstr__(register char const *const src) {
  return strcpy(imalloct("String", (strlen(src) + 1u) * sizeof(char)), src);
}

PRIVATE void __Constructor__(register void *const _self,
                             register struct ImParams *const args) {
  register struct FileHttpHandler *const self = _self;
  if (ImParams_Match(args, 1u, PARAM_PTR) == IM_FALSE) {
    impanic("FileHttpHandler Constructor takes (char const *)");
  }
  ImParams_Extract(args, &self->public_path);
  self->public_path = __dupstr__(self->public_path);
}

PRIVATE void __Destructor__(register void *const _self) {
  register struct FileHttpHandler *const self = _self;
  imfree((void *)self->public_path);
}

PRIVATE void GenerateFileResponse(register struct HttpResponse *const response,
                                  register char const *const path) {
  register char const *const file_content = ReadEntireFile(path);
  HttpResponse_SetMimeType(response, GetMimeTyeFromPath(path));
  ImStr_Append(HttpResponse_GetBody(response), file_content);
  HttpResponse_Finalize(response);
  imfree((void *)file_content);
}

PRIVATE struct ImOptPtr
__Handle__(register void *const _self,
           register struct HttpRequest *const request) {
  register struct FileHttpHandler *const self = _self;
  register struct HttpResponse *const response = imnew(HttpResponse, 0u);
  register struct ImStr *const path =
      imnew(ImStr, 1u, PARAM_PTR, self->public_path);
  register char const *req_path = HttpRequest_GetPath(request);

  if (strcmp(req_path, "/") == 0) {
    ImStr_Append(path, "/index.html");
  } else {
    ImStr_Append(path, req_path);
  }

  if (access(ImStr_View(path), F_OK) == 0) {
    GenerateFileResponse(response, ImStr_View(path));
    (void)imdel(path);
    return ImOptPtr_Some(response);
  }

  ImStr_Append(path, ".htm");
  if (access(ImStr_View(path), F_OK) == 0) {
    GenerateFileResponse(response, ImStr_View(path));
    (void)imdel(path);
    return ImOptPtr_Some(response);
  }

  ImStr_Append(path, "l");
  if (access(ImStr_View(path), F_OK) == 0) {
    GenerateFileResponse(response, ImStr_View(path));
    (void)imdel(path);
    return ImOptPtr_Some(response);
  }

  (void)imdel(path);
  (void)imdel(response);
  return ImOptPtr_None();
}

PRIVATE void __InterfaceImplementation__(register void *const interface) {
  if (imisof(interface, HttpHandler) != IM_FALSE) {
    register struct HttpHandler *const handler_interface = interface;
    handler_interface->handle = __Handle__;
  } else {
    impanic("FileHttpHandler does not implement %s\n", imtype(interface));
  }
}

CLASS(FileHttpHandler) {
  _FileHttpHandler.size = sizeof(struct FileHttpHandler);
  _FileHttpHandler.ctor = __Constructor__;
  _FileHttpHandler.dtor = __Destructor__;
  _FileHttpHandler.implof = __InterfaceImplementation__;
}
