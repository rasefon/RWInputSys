#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
/*#include "common.h"*/

#define BUFF_SIZE       2048
#define READ_SIZE       (2048-1)
#define BUFF_END        (&_curr_buffer[BUFF_SIZE])

typedef unsigned char   uchar;
typedef unsigned long   ulong;
// Using 2 buffers to cache the input strings
static uchar    _buffer_1[BUFF_SIZE];
static uchar    _buffer_2[BUFF_SIZE];
static uchar    *_curr_buffer = _buffer_1;
static uchar    *_backup_buffer = _buffer_2;
// using 1 buffer when meeting special case.
// If the next token is beyond current buffer, we need to read more character from the other buffer. After determining the
// position of the other buffer, we should copy the characters from both buffer 1 and buffer 2 to this buff, so that the 
// next token can be used by parser or lexer.
static uchar    _cat_buffer[BUFF_SIZE * 2];
static uchar    *_pre_token_start = NULL;
static uchar    *_curr_token_start = NULL;
static uchar    *_curr_token_end = NULL;
static ulong    _pre_token_len = 0;
static ulong    _curr_token_len = 0;
static ulong    _curr_token_index = 0;
static ulong    _curr_lineno = 1; // current line number when parsing input buffer.
static int      _curr_fd = 0;

static void clear_buffers();
static void fill_buffers();

int rw_openfile(const char *fn)
{
   int ret_val;
   if (ret_val = open(fn, O_RDONLY | O_BINARY)) {
      _curr_fd = ret_val;
      _curr_buffer = _buffer_1;
      _backup_buffer = _buffer_2;
      _pre_token_start = NULL;
      _curr_token_start = NULL;
      _curr_token_end = NULL;
      _curr_token_len = 0;
      _pre_token_len = 0;
      _curr_token_index = 0;
      _curr_lineno = 1;

      // clear buffers.
      clear_buffers();
      
      // Read file
      fill_buffers();
     
   }
   return ret_val;
}

static void fill_buffers()
{
   ssize_t read_amout = read(_curr_fd, _curr_buffer, READ_SIZE);
   if (read_amout == READ_SIZE) {
      _curr_buffer[READ_SIZE] = EOF;
      read_amout = read(_curr_fd, _backup_buffer, READ_SIZE);
      if (read_amout == READ_SIZE) {
         _backup_buffer[READ_SIZE] = EOF;
      }
   }
}

static void clear_buffers()
{
   memset(_buffer_1, 0, BUFF_SIZE);
   memset(_buffer_2, 0, BUFF_SIZE);
   memset(_cat_buffer, 0, BUFF_SIZE);
}


// Test main function
int main(int argc, char** argv)
{
   rw_openfile("si.rb");
   printf("%x", _curr_buffer[READ_SIZE]);

   return 1;
}

