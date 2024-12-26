#ifndef API_LOGIN_H_
#define API_LOGIN_H_

#include "imlib/imclass.h"

#include "../fordodb/fordodb.h"

struct LoginApiHandler {
  struct FordoDB *db;
};

extern struct ImClass *const LoginApiHandler;

#endif /* !API_LOGIN_H_ */

