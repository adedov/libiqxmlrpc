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
//  $Id: sigsock.h,v 1.2 2004-04-27 05:25:09 adedov Exp $

#ifndef _libiqnet_sigsock_h_
#define _libiqnet_sigsock_h_

#include "reactor.h"
#include "lock.h"

namespace iqnet 
{
  class Alarm_socket;
};


//! Helper pair of sockets which allows to reset poll/select
//! state from another thread.
class iqnet::Alarm_socket: public iqnet::Event_handler {
  int sock[2];
  Reactor* reactor;
  Lock* lock;

public:
  Alarm_socket( Reactor*, Lock* );
  ~Alarm_socket();

  void handle_input( bool& );

  int get_fd() const { return sock[0]; }

  void send_alarm();
};

#endif
