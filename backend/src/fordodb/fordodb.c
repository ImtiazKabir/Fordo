#include "fordodb.h"

#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/imodlog.h"
#include "imlib/ansi.h"
#include "imlib/imlog.h"
#include "imlib/impanic.h"

#include <sqlite3.h>

struct FordoDB {
  sqlite3 *db;
  char const *add_user_script;
};

struct IModLog db_logger = {
  (size_t)-1,
  {
    {DB_INSERT, "[INSERT]", ANSI_BG_DEFAULT, ANSI_FG_GREEN, ANSI_BG_DEFAULT, ANSI_FG_GREEN},
    {DB_DELETE, "[DELETE]", ANSI_BG_DEFAULT, ANSI_FG_RED, ANSI_BG_DEFAULT, ANSI_FG_RED},
    {DB_UPDATE, "[UPDATE]", ANSI_BG_DEFAULT, ANSI_FG_YELLOW, ANSI_BG_DEFAULT, ANSI_FG_YELLOW},
  }
};

PRIVATE size_t SizeOfFile(register FILE *const fp) {
  register long size = 0u;

  NEQ(fseek(fp, 0, SEEK_END), -1);
  NEQ(size = ftell(fp), -1);
  NEQ(fseek(fp, 0, SEEK_SET), -1);

  return (size_t) size;
}

PRIVATE char *ReadEntireFile(register char const *const file) {
  register FILE *fp = NULL;
  register char *content = NULL;
  register size_t size = 0;

  NEQ(fp = fopen(file, "r"), NULL);
  size = SizeOfFile(fp);
  NEQ(content = calloc(1u + size, sizeof(*content)), NULL);
  NEQ(fread(content, sizeof(*content), size, fp), 0);
  EQ(fclose(fp), 0);
  content[size] = '\0';

  return content;
}

PRIVATE void __Constructor__(void *_self, struct ImParams *args) {
  register struct FordoDB *const self = _self;
  auto char const *dbFilePath = NULL;
  
  ImParams_Extract(args, &dbFilePath);
  imlog1(LOG_INFO, "Constructing the database from %s", dbFilePath);

  EQ(sqlite3_open(dbFilePath, &self->db), SQLITE_OK);

  imlog(LOG_INFO, "Reading scripts");
  NEQ(self->add_user_script = ReadEntireFile("database/add_user.sql"), NULL);
  imlog(LOG_INFO, "Successfully read all scripts");
}

PRIVATE void __Destructor__(void *_self) {
  register struct FordoDB *const self = _self;

  free((void *)self->add_user_script);
  EQ(sqlite3_close(self->db), SQLITE_OK);
}

PUBLIC void FordoDB_AddUserToDB(
  register struct FordoDB *const self,
  register char const *const username,
  register char const *const password
) {
  auto sqlite3_stmt *stmt = NULL;
  register char const *const script = self->add_user_script;
  register sqlite3 *const db = self->db;

  if (sqlite3_prepare_v2(db, script, -1, &stmt, NULL) != SQLITE_OK) {
    imlogf1(LOG_ERROR, stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return;
  }

  if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
    imlogf1(LOG_ERROR, stderr, "Failed to bind username: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return;
  }

  if (sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC) != SQLITE_OK) {
    imlogf1(LOG_ERROR, stderr, "Failed to bind password: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    imlogf1(LOG_ERROR, stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return;
  }

  imodlog(&db_logger, DB_INSERT, "Added username: %s\n", username);

  sqlite3_finalize(stmt);
}


CLASS(FordoDB) {
  _FordoDB.size = sizeof(struct FordoDB);
  _FordoDB.ctor = __Constructor__;
  _FordoDB.dtor = __Destructor__;
}


