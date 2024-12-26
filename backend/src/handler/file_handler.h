#ifndef FILE_HTTP_HANDLER_H_
#define FILE_HTTP_HANDLER_H_

#include "imlib/imclass.h"

struct FileHttpHandler {
  char const *public_path;
};

extern struct ImClass *const FileHttpHandler;

#endif /* !FILE_HTTP_HANDLER_H_ */
