#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include <stdio.h>
#include <stdlib.h>

#include "imlib/imstdinc.h"

PUBLIC size_t SizeOfFile(register FILE *const fp);
PUBLIC char *ReadEntireFile(register char const *const file);

#endif  /* !FILE_UTIL_H_ */
