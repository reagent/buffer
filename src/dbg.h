#ifndef __dbg_h
#define __dbg_h

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifndef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s (in function '%s'):%d:  " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" :  strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s (in function '%s'):%d: errno: %s) " M "\n", __FILE__, __FUNCTION__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define jump_to_error_if(A) if (A) { goto error; }
#define jump_to_error_unless(A) if (!(A)) { goto error; }

#endif