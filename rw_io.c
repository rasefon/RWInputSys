#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "common.h"

#define BUFF_SIZE       4096
#define BACKUP_SIZE     (BUFF_SIZE*2)
#define BUFF_END        (&_curr_buffer[BUFF_SIZE])

typedef unsigned char   uchar;
typedef unsigned long   ulong;
// Using 2 buffers to cache the input strings
static uchar      _buffer_1[BUFF_SIZE];
static uchar      _buffer_2[BUFF_SIZE];
static uchar      *_curr_buffer = _buffer_1;
static uchar      *_standby_buffer = _buffer_2;
static uchar      _backup_buffer[BACKUP_SIZE];

/*static int        _pre_token_start = 0;*/
/*static int        _pre_token_end = 0;*/
static int        _curr_token_start = 0;
static int        _curr_token_end = 0;
static int        _next_char_i = 0;
static int        _curr_lineno = 1; // current line number when parsing input buffer.
static int        _curr_fd = 0;
static bool       _use_backup_buffer = false; // flag to determine whether to use concatence buffer.
static int        _backup_len = 0;

static void clear_buffers();
static int fill_buffers();

uchar rw_advance()
{
   // make 1 step forward and return the character.
   if (_next_char_i++ < BUFF_SIZE) {
      if(_curr_buffer[_next_char_i] != EOF) {
         return _curr_buffer[_next_char_i];
      }
      else {
         return 0;
      }
   }
   else {
      // at the end of current buffer.
      // first, copy the current scanning token to the backup buffer.
      _backup_len = BUFF_SIZE - _curr_token_start;
      memcpy(_backup_buffer, &_curr_buffer[_curr_token_start], _backup_len);
      _use_backup_buffer = true; // flag of using backup buffer.
      _curr_token_start = 0;
      // If the _standyb_buffer is not NULL, use the standby buffer first.
      if (_standby_buffer) {
         _curr_buffer = _standby_buffer;
         _standby_buffer = NULL;
         _next_char_i = 0;
         return _curr_buffer[_next_char_i];
      }
      else {
         // If standby buffer is NULL, read characters from file.
         if (fill_buffers()) {
            _next_char_i = 0;
            return _curr_buffer[_next_char_i];
         }
         else {
            // can't get any characters from file.
            return 0;
         }
      }
   }
}

void begin_curr_scan()
{
   if (!_curr_buffer) {
      // error!
      DBG( printf("Error when beginning current token scanning!\n"); )
   }

   _curr_token_start = _next_char_i = _curr_token_end + 1;
   _use_backup_buffer = false;
}

void end_curr_scan()
{
   if (!_curr_buffer) {
      // error!
      DBG( printf("Error when ending current token scanning!\n"); )
   }

   if (_use_backup_buffer) {
      // copy the remained characters to backup buffer.
      memcpy(&_backup_buffer[_backup_len], _curr_buffer, _next_char_i);
      _curr_token_end = _backup_len + _next_char_i;
   }
   else {
      _curr_token_end = _next_char_i;
   }
}

uchar* rw_curr_token() {
   if (!_curr_buffer) {
      // error!
      DBG( printf("Error when getting current token!\n"); )
   }

   if (_use_backup_buffer) {
      return _backup_buffer;
   }
   else {
      return &_curr_buffer[_curr_token_start];
   }
}

int rw_curr_token_len() 
{
   return _curr_token_end - _curr_token_start;
}

int rw_openfile(const char *fn)
{
   int ret_val;
   if (ret_val = OPEN(fn, O_RDONLY | O_BINARY)) {
      _curr_fd = ret_val;
      _curr_buffer = _buffer_1;
      _standby_buffer = _buffer_2;
      _curr_token_start = 0;
      _curr_token_end = 0;
      _next_char_i = -1;
      _curr_lineno = 1;
      _use_backup_buffer = false;
      _backup_len = 0;

      // clear buffers.
      clear_buffers();
      
      // Read file
      fill_buffers();
   }
   return ret_val;
}

static int fill_buffers()
{
   _curr_buffer = _buffer_1;
   _standby_buffer = _buffer_2;
   int ret_val;
   ssize_t read_amout = READ(_curr_fd, _curr_buffer, BUFF_SIZE);
   if (read_amout == BUFF_SIZE) {
      read_amout = READ(_curr_fd, _standby_buffer, BUFF_SIZE);
      ret_val = 1;
   }
   else if (read_amout <= 0) {
      ret_val = -1;
      _curr_buffer = NULL;
      _standby_buffer = NULL;
   }

   return ret_val;
}

static void clear_buffers()
{
   memset(_curr_buffer, 0, BUFF_SIZE);
   memset(_standby_buffer, 0, BUFF_SIZE);
   memset(_backup_buffer, 0, BACKUP_SIZE);
}

// Test main function
int main(int argc, char** argv)
{
   rw_openfile("rw_io.c");

   return 1;
}

