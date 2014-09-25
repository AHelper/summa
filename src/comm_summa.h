#ifndef COMM_SUMMA
#define COMM_SUMMA

#include "comm_summa.pb-c.h"

int comm_summa_open(const char* path);
CommSummaRequest comm_summa_read_request(int fd);
void comm_summa_send_query(int fd, const CommSummaQuery& query);
void comm_summa_send_set(int fd, const CommSummaSet& set);

#endif // COMM_SUMMA