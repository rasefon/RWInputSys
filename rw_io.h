#ifndef  _RW_IO_H_
#define  _RW_IO_H_

unsigned char rw_advance();
unsigned char rw_lookforward(int n);
void rw_begin_curr_scan();
void rw_end_curr_scan();
unsigned char* rw_curr_token();
int rw_curr_token_len();
int rw_curr_lineno();
int rw_is_top();
int rw_openfile(const char *fn)

#endif
