//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

/*! \file sysinc.h
    This file should help to port library.
    Insert here include macro of platform dependent headers.
*/

#ifndef _iqxmlrpc_sysinc_h_
#define _iqxmlrpc_sysinc_h_

#ifdef _WIN32
  #define BOOST_ALL_NO_LIB

#if _MSC_VER < 1700
  #include <Winsock2.h>
  #include <windows.h>
  #include <ws2tcpip.h>
#endif

#else
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <sys/types.h>
  #include <pthread.h>
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
#endif //_WIN32

#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#endif
