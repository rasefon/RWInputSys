#ifndef _COMMON_H
#define _COMMON_H

#ifdef _WIN32
#define  OPEN   _open
#define  CLOSE  _close
#define  READ   _read
#define  O_RDONLY    _O_RDONLY
#define  O_BINARY    _O_BINARY
#else
#define  OPEN  open
#define  CLOSE close
#define  READ  read
#ifndef  O_BINARY
#define  O_BINARY 0
#endif
#endif

#ifdef RWDEBUG
#define  DBG(x)  x
#else
#define  DBG(x)   
#endif

#endif
