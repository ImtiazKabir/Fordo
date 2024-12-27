#ifndef API_SIGNUP_H_
#define API_SIGNUP_H_

#include "imlib/imclass.h"

#include "../fordodb/fordodb.h"

struct SignupApiHandler {
  struct FordoDB *db;
};

extern struct ImClass *const SignupApiHandler;

#endif /* !API_SIGNUP_H_ */

