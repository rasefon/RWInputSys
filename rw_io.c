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
static int        _token_lineno = 1;
static int        _curr_fd = 0;
static bool       _use_backup_buffer = false; // flag to determine whether to use concatence buffer.
static int        _backup_len = 0;
static bool       _is_top = true;

static void clear_buffers();
static int fill_buffers();

uchar rw_advance()
{
   uchar ret_val = '\0';
   if (_next_char_i < BUFF_SIZE && _curr_buffer[_next_char_i] == '\n') {
      _curr_lineno++;
   }

   // make 1 step forward and return the character.
   if (_next_char_i < BUFF_SIZE) {
      if(_curr_buffer[_next_char_i] != EOF) {
         ret_val = _curr_buffer[_next_char_i];
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
      if (fill_buffers()) {
         _next_char_i = 0;
         ret_val = _curr_buffer[_next_char_i];
      }
   }
   ++_next_char_i;
   return ret_val;
}

uchar rw_lookforward(int n)
{
   uchar ret_val = '\0';
   int look_pos = _next_char_i+n-1;
   if (look_pos < BUFF_SIZE) {
      // look forward steps is within current buffer.
      ret_val = _curr_buffer[look_pos];
   }
   else {
      /*look forword steps is beyond current buffer.*/
      /*I didn't handle the bug about too long look forward steps, ( >= 2*4096) cause it's impratical.*/
      if (!_standby_buffer) {
         ssize_t read_amount = READ(_curr_fd, _standby_buffer, BUFF_SIZE);
         look_pos -= BUFF_SIZE;
         if (read_amount > 0 && read_amount > look_pos ) {
            ret_val = _curr_buffer[look_pos];
         }
      }
   }

   return ret_val;
}

 /*when scanning '^' at the beginning of files, use this function to check.*/
bool rw_is_top() 
{ 
   if (_is_top && _next_char_i == 0) {
      return true;
   }
   else {
      _is_top = false;
      return false;
   }
}

void rw_begin_curr_scan()
{
   if (!_curr_buffer) {
      // error!
      DBG( printf("Error when beginning current token scanning!\n"); )
   }

   _curr_token_start = _next_char_i = _curr_token_end + 1;
   _use_backup_buffer = false;
}

void rw_end_curr_scan()
{
   if (!_curr_buffer) {
      // error!
      DBG( printf("Error when ending current token scanning!\n"); )
   }

   _token_lineno = _curr_lineno;

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

int rw_curr_lineno() { return _token_lineno; }

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
      _token_lineno = 1;
      _use_backup_buffer = false;
      _backup_len = 0;
      _is_top = true;

      // clear buffers.
      clear_buffers();
      
      // Read file
      fill_buffers();
   }
   return ret_val;
}

static int fill_buffers()
{
   int ret_val = 1;
   if(_standby_buffer) {
      _curr_buffer = _standby_buffer;
      _standby_buffer = NULL;
   }
   else {
      _curr_buffer = _buffer_1;
      _standby_buffer = _buffer_2;
      ssize_t read_amount = READ(_curr_fd, _curr_buffer, BUFF_SIZE);
      if (read_amount == BUFF_SIZE) {
         read_amount = READ(_curr_fd, _standby_buffer, BUFF_SIZE);
      }
      else if (read_amount <= 0) {
         ret_val = 0;
         _curr_buffer = NULL;
         _standby_buffer = NULL;
      }
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
   rw_begin_curr_scan();
   for (int i = 0; i < 4200; i++) {
      if (!rw_advance()) {
         break;
      }
   }
   rw_end_curr_scan();

   return 1;
}

