//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
//  
//  $Id: sysinc.h,v 1.3 2004-04-27 04:16:51 adedov Exp $

/*! \file sysinc.h 
    This file should help to port library.
    Insert here include macro of platform dependent headers.
*/

#ifndef _iqxmlrpc_sysinc_h_
#define _iqxmlrpc_sysinc_h_

#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>

#ifdef HAVE_POLL
  #include <sys/poll.h>
#endif

#endif
