#ifndef __SHARED_DEFINES_H__
#define __SHARED_DEFINES_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define assertf(A, M, ...) if(!(A)) {log_error(M, ##__VA_ARGS__); assert(A); }

#define DEBUG 1		// Enable Debug Mode

#define INVALID -325 // Invalid value


#endif
