#ifndef FORDO_DB_H_
#define FORDO_DB_H_

#include "imlib/imclass.h"
#include "imlib/imodlog.h"

struct FordoDB;
extern struct ImClass *const FordoDB;


enum DBLogLevel {
  DB_INSERT,
  DB_DELETE,
  DB_UPDATE
};
extern struct IModLog db_logger;

PUBLIC void FordoDB_AddUserToDB(struct FordoDB *self, char const *username, char const *password);

#endif /* !FORDO_DB_H_ */

