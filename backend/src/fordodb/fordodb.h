#ifndef FORDO_DB_H_
#define FORDO_DB_H_

#include "imlib/imclass.h"
#include "imlib/imodlog.h"
#include "imlib/imerror.h"
#include "imlib/imresult.h"

struct FordoDB;
extern struct ImClass *const FordoDB;

enum DBLogLevel {
  DB_INSERT,
  DB_DELETE,
  DB_UPDATE
};
extern struct IModLog db_logger;

enum DBErrorStatus {
  DB_OK = 0,
  DB_ERR = -1,
  DB_PREP = -2,
  DB_BIND = -3,
  DB_EXEC = -4
};

IM_DECLARE_ERROR(DatabaseError, ImError)
IM_DECLARE_ERROR(PrepareError, DatabaseError)
IM_DECLARE_ERROR(BindError, DatabaseError)
IM_DECLARE_ERROR(ExecuteError, DatabaseError)

IM_DECLARE_RESULT(DBResult, int)

PUBLIC struct DBResult FordoDB_AddUserToDB(
    struct FordoDB *self,
    char const *username,
    char const *password
);

#endif /* !FORDO_DB_H_ */

