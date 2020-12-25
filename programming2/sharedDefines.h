#ifndef __SHARED_DEFINES_H__
#define __SHARED_DEFINES_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define assertf(A, M, ...) if(!(A)) {log_error(M, ##__VA_ARGS__); assert(A); }

#define DEBUG 1	// Enable Debug Mode

#if DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */

// +-----------------------+--------+
// | EVENT TYPE:           | VALUE  |
// +-----------------------+--------+
// | SERVER				   |    1   |
// | START_LEADER_ELECTION |    2   |
// | CONNECT               |    3   |
// | ORDER				   |    4   | 
// | SUPPLY                |    5   | 
// | PRINT                 |    6   |
// | EXTERNAL SUPPLY       |    7   |
// | REPORT                |    8   |
// | PROB                  |    9   |
// | REPLY                 |    10  |
// | TERMINATE             |    11  |
// +-----------------------+--------+
#define SERVER				  1
#define START_LEADER_ELECTION 2
#define CONNECT               3
#define ORDER				  4
#define SUPPLY                5
#define PRINT                 6
#define EXTERNAL SUPPLY       7
#define REPORT                8
#define PROB                  9
#define REPLY                 10
#define LEADER                11
#define NEIGHBOR              12
#define ACK                   255
#define EXIT                  425

struct _msg {
	int pid;			// Process Id 
	int val_1;			// Value
	int val_2;			// Value
	int val_3;			// Value
	int val_4;			// Value
};

void my_send(struct _msg *msg, int rank, int tag);
void my_receive(struct _msg *msg, int rank);
struct _msg prepare_msg(int pid, int val_1, int val_2, int val_3, int val_4);


#endif
