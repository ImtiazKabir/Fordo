#include "fordodb.h"

#include "imlib/ansi.h"
#include "imlib/imclass.h"
#include "imlib/imclass_prot.h"
#include "imlib/imerror.h"
#include "imlib/imlog.h"
#include "imlib/imodlog.h"
#include "imlib/impanic.h"
#include "imlib/imresult.h"
#include "imlib/imstr.h"

#include <sqlite3.h>

IM_DEFINE_ERROR(DatabaseError, DB_OK, "Internal database error")
IM_DEFINE_ERROR(PrepareError, DB_PREP, "Could not prepare statement")
IM_DEFINE_ERROR(BindError, DB_PREP, "Could not bind variable")
IM_DEFINE_ERROR(ExecuteError, DB_EXEC, "Could not execute statement")

struct FordoDB {
  sqlite3 *db;

  char const *foreign_key_script;

  char const *add_user_script;
  char const *add_todo_script;
  char const *delete_todo_script;
  char const *get_user_id_script;
  char const *get_todos_script;
  char const *toggle_todo_script;
};

struct IModLog db_logger = {
    (size_t)-1,
    {
        {DB_INSERT, "[INSERT]", ANSI_BG_DEFAULT, ANSI_FG_GREEN, ANSI_BG_DEFAULT,
         ANSI_FG_GREEN},
        {DB_DELETE, "[DELETE]", ANSI_BG_DEFAULT, ANSI_FG_RED, ANSI_BG_DEFAULT,
         ANSI_FG_RED},
        {DB_UPDATE, "[UPDATE]", ANSI_BG_DEFAULT, ANSI_FG_YELLOW,
         ANSI_BG_DEFAULT, ANSI_FG_YELLOW},
    }};

PRIVATE size_t SizeOfFile(register FILE *const fp) {
  register long size = 0u;

  NEQ(fseek(fp, 0, SEEK_END), -1);
  NEQ(size = ftell(fp), -1);
  NEQ(fseek(fp, 0, SEEK_SET), -1);

  return (size_t)size;
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

PRIVATE struct ImStr *ErrorMessage(register sqlite3 *const db,
                                   register char const *const prefix) {
  register struct ImStr *str = imnew(ImStr, 0u);
  ImStr_AppendFmt(str, "%s: %s", prefix, sqlite3_errmsg(db));
  return str;
}

PRIVATE void
EnableForeignKeyConstraint(register struct FordoDB const *const self) {
  auto char *errmsg = NULL;
  register sqlite3 *const db = self->db;
  register char const *const script = self->foreign_key_script;

  if (sqlite3_exec(db, script, NULL, NULL, &errmsg) != SQLITE_OK) {
    imlogf1(LOG_WARN, stderr, "Failed to enable foreign key constraint: %s",
            errmsg);
    sqlite3_free(errmsg);
  }
}

PRIVATE void __Constructor__(void *_self, struct ImParams *args) {
  register struct FordoDB *const self = _self;
  auto char const *dbFilePath = NULL;

  ImParams_Extract(args, &dbFilePath);
  imlog1(LOG_INFO, "Constructing the database from %s", dbFilePath);

  EQ(sqlite3_open(dbFilePath, &self->db), SQLITE_OK);

  imlog(LOG_INFO, "Reading scripts");

  NEQ(self->foreign_key_script =
          ReadEntireFile("database/enable_foreign_key.sql"),
      NULL);

  NEQ(self->add_user_script = ReadEntireFile("database/add_user.sql"), NULL);
  NEQ(self->add_todo_script = ReadEntireFile("database/add_todo.sql"), NULL);
  NEQ(self->get_user_id_script = ReadEntireFile("database/get_user_id.sql"),
      NULL);
  NEQ(self->get_todos_script = ReadEntireFile("database/get_todos.sql"), NULL);
  NEQ(self->delete_todo_script = ReadEntireFile("database/delete_todo.sql"), NULL);
  NEQ(self->toggle_todo_script = ReadEntireFile("database/toggle_todo.sql"),
      NULL);

  imlog(LOG_INFO, "Enabling constraints");
  EnableForeignKeyConstraint(self);
}

PRIVATE void __Destructor__(void *_self) {
  register struct FordoDB *const self = _self;

  imlog(LOG_INFO, "Cleaning up database");

  free((void *)self->foreign_key_script);

  free((void *)self->add_user_script);
  free((void *)self->add_todo_script);
  free((void *)self->get_user_id_script);
  free((void *)self->get_todos_script);
  free((void *)self->delete_todo_script);
  free((void *)self->toggle_todo_script);

  EQ(sqlite3_close(self->db), SQLITE_OK);
}

PUBLIC struct ImResInt FordoDB_AddUser(register struct FordoDB *const self,
                                        register char const *const username,
                                        register char const *const password) {
  auto sqlite3_stmt *stmt = NULL;
  register char const *const script = self->add_user_script;
  register sqlite3 *const db = self->db;

  if (sqlite3_prepare_v2(db, script, -1, &stmt, NULL) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to prepare statement");
    register struct ImError *const error =
        imnew(PrepareError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind username");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind password");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to execute statement");
    register struct ImError *const error =
        imnew(ExecuteError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  {
    register int const user_id = (int)sqlite3_last_insert_rowid(db);
    imodlog(&db_logger, DB_INSERT, "Added username: %s as user %d\n", username, user_id);
    sqlite3_finalize(stmt);
    return ImResInt_Ok(user_id);
  }
}

PUBLIC struct ImResInt FordoDB_AddTodo(register struct FordoDB *const self,
                                        register int const user_id,
                                        register char const *const text) {
  auto sqlite3_stmt *stmt = NULL;
  register char const *const script = self->add_todo_script;
  register sqlite3 *const db = self->db;

  if (sqlite3_prepare_v2(db, script, -1, &stmt, NULL) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to prepare statement");
    register struct ImError *const error =
        imnew(PrepareError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_int(stmt, 1, user_id) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind user id");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_text(stmt, 2, text, -1, SQLITE_STATIC) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind todo text");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to execute statement");
    register struct ImError *const error =
        imnew(ExecuteError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  {
    register int const todo_id = (int)sqlite3_last_insert_rowid(db);
    imodlog(&db_logger, DB_INSERT, "Added todo %d for user %d\n", todo_id,
            user_id);

    sqlite3_finalize(stmt);
    return ImResInt_Ok(todo_id);
  }
}

PUBLIC struct ImResInt FordoDB_GetUserId(register struct FordoDB *const self,
                                         register char const *const username,
                                         register char const *const password) {
  auto sqlite3_stmt *stmt = NULL;
  register char const *const script = self->get_user_id_script;
  register sqlite3 *const db = self->db;

  if (sqlite3_prepare_v2(db, script, -1, &stmt, NULL) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to prepare statement");
    register struct ImError *const error =
        imnew(PrepareError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind username");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind password");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_step(stmt) != SQLITE_ROW) {
    register char const *const errmsg = "Invalid credentials";
    register struct ImError *const error =
        imnew(ExecuteError, 1u, PARAM_PTR, errmsg);
    register struct ImResInt result = ImResInt_Err(error);
    imlogf(LOG_ERROR, stderr, errmsg);

    sqlite3_finalize(stmt);
    return result;
  }

  {
    register int const user_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return ImResInt_Ok(user_id);
  }
}

PUBLIC struct ImResVoid FordoDB_DeleteTodo(register struct FordoDB *const self,
                                        register int const todo_id) {
  auto sqlite3_stmt *stmt = NULL;
  register char const *const script = self->delete_todo_script;
  register sqlite3 *const db = self->db;

  if (sqlite3_prepare_v2(db, script, -1, &stmt, NULL) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to prepare statement");
    register struct ImError *const error =
        imnew(PrepareError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResVoid result = ImResVoid_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_int(stmt, 1, todo_id) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind user id");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResVoid result = ImResVoid_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to execute statement");
    register struct ImError *const error =
        imnew(ExecuteError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResVoid result = ImResVoid_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  imodlog(&db_logger, DB_DELETE, "Deleted todo id: %d\n", todo_id);

  sqlite3_finalize(stmt);
  return ImResVoid_Ok(DB_OK);
}

PUBLIC struct ImResVoid FordoDB_ToggleTodo(register struct FordoDB *const self,
                                        register int const todo_id) {
  auto sqlite3_stmt *stmt = NULL;
  register char const *const script = self->toggle_todo_script;
  register sqlite3 *const db = self->db;

  if (sqlite3_prepare_v2(db, script, -1, &stmt, NULL) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to prepare statement");
    register struct ImError *const error =
        imnew(PrepareError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResVoid result = ImResVoid_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_bind_int(stmt, 1, todo_id) != SQLITE_OK) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to bind user id");
    register struct ImError *const error =
        imnew(BindError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResVoid result = ImResVoid_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    register struct ImStr *const errmsg =
        ErrorMessage(db, "Failed to execute statement");
    register struct ImError *const error =
        imnew(ExecuteError, 1u, PARAM_PTR, ImStr_View(errmsg));
    register struct ImResVoid result = ImResVoid_Err(error);
    imlogf(LOG_ERROR, stderr, ImStr_View(errmsg));

    sqlite3_finalize(stmt);
    imdel(errmsg);
    return result;
  }

  imodlog(&db_logger, DB_UPDATE, "Toggled todo id: %d\n", todo_id);

  sqlite3_finalize(stmt);
  return ImResVoid_Ok(DB_OK);
}


CLASS(FordoDB) {
  _FordoDB.size = sizeof(struct FordoDB);
  _FordoDB.ctor = __Constructor__;
  _FordoDB.dtor = __Destructor__;
}
