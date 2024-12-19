#include "imlib/imlog.h"
#include "imlib/impanic.h"
#include "imlib/imstdinc.h"
#include "imlib/imerrno.h"

#include <sqlite3.h>
#include <stdlib.h>

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

  return content;
}

PRIVATE void start(void) {
  register char *const content = ReadEntireFile("database/add_user.sql");
  (void)printf("%s\n", content);
  free(content);
}


PUBLIC int main(register int const argc, register char const *const *const argv) {
  (void)argc;
  trace_target = argv[0];
  imclrerr();
  start();
  return EXIT_SUCCESS;
}

