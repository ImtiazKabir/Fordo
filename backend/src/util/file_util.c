#include "file_util.h"

#include <stdlib.h>
#include <stdio.h>

#include "imlib/imstdinc.h"
#include "imlib/impanic.h"

PUBLIC size_t SizeOfFile(register FILE *const fp) {
  register long size = 0u;

  NEQ(fseek(fp, 0, SEEK_END), -1);
  NEQ(size = ftell(fp), -1);
  NEQ(fseek(fp, 0, SEEK_SET), -1);

  return (size_t)size;
}

PUBLIC char *ReadEntireFile(register char const *const file) {
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
