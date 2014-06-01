#ifndef  _RW_IO_H_
#define  _RW_IO_H_

unsigned char rw_advance();
void begin_curr_scan();
void end_curr_scan();
unsigned char* rw_curr_token();
int rw_curr_token_len();
int rw_openfile(const char *fn)

#endif
