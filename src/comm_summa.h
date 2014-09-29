#ifndef COMM_SUMMA
#define COMM_SUMMA

#include "comm_summa.pb-c.h"
#include "ret_code.h"

RET_CODE comm_summa_open(const char* path, int *sock);
RET_CODE comm_summa_close(int fd);
RET_CODE comm_summa_accept(int fd, int *client);
RET_CODE comm_summa_read_request(int fd, CommSummaRequest** request);
RET_CODE comm_summa_send_query(int fd, const CommSummaQuery** query);
RET_CODE comm_summa_send_set(int fd, const CommSummaSet** set);

#endif // COMM_SUMMA