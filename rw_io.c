#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "common.h"

#define BUFF_SIZE       2048
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
static int      _pre_token_start = 0;
static int      _curr_token_start = 0;
static int      _curr_token_end = 0;
static int      _next_char = 0;
static int      _curr_lineno = 1; // current line number when parsing input buffer.
static int      _curr_fd = 0;
static bool     _use_cat_buffer = false; // flag to determine whether to use concatence buffer.

static void clear_buffers();
static void fill_buffers();

int rw_openfile(const char *fn)
{
   int ret_val;
   if (ret_val = open(fn, O_RDONLY | O_BINARY)) {
      _curr_fd = ret_val;
      _curr_buffer = _buffer_1;
      _backup_buffer = _buffer_2;
      _pre_token_start = 0;
      _curr_token_start = 0;
      _curr_token_end = 0;
      _next_char = 0;
      _curr_lineno = 1;
      _use_cat_buffer = false;

      // clear buffers.
      clear_buffers();
      
      // Read file
      fill_buffers();
   }
   return ret_val;
}

static void fill_buffers()
{
   _curr_buffer = _buffer_1;
   _backup_buffer = _buffer_2;
   ssize_t read_amout = read(_curr_fd, _curr_buffer, BUFF_SIZE);
   if (read_amout == BUFF_SIZE) {
      read_amout = read(_curr_fd, _backup_buffer, BUFF_SIZE);
   }
}

uchar rw_advance()
{
   uchar ret_val;
   // make 1 step forward and return the character.
   if ( _next_char < BUFF_SIZE) {
      if(_curr_buffer[_next_char] != EOF) {
         ret_val = _curr_buffer[_next_char];
      }
      else {
         ret_val = 0;
      }
   }
   else {
      // if backup buffer hadn't been used, swith to the backup buffer, otherwise, fill the buffers and copy previous 
      // scanned token and current scanning token into concatence buffer.
      int cat_buff_len = BUFF_SIZE-_pre_token_start;
      memcpy(_cat_buffer, &_curr_buffer[_pre_token_start], cat_buff_len);
      int pre_token_len = _curr_token_start - _pre_token_start;
      _pre_token_start = 0;
      _curr_token_start = pre_token_len;
      _use_cat_buffer = true;

      if (_backup_buffer != NULL) {
         _curr_buffer = _backup_buffer;
         _backup_buffer = NULL;
      }
      else {
         // Get characters from file.
         fill_buffers();
         _next_char = 0;
      }
      ret_val = _curr_buffer[_next_char];
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
   rw_openfile("rw_io.c");

   return 1;
}

