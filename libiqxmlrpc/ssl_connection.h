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
//  $Id: ssl_connection.h,v 1.1 2004-04-22 09:25:56 adedov Exp $

#ifndef _libiqnet_ssl_connection_
#define _libiqnet_ssl_connection_

#include <openssl/ssl.h>
#include "connection.h"
#include "ssl_lib.h"
#include "conn_fabric.h"


namespace iqnet
{
  namespace ssl 
  {
    class Connection;
    class Reaction_connection;
    class Serial_conn_fabric;
  }
};
  

//! SSL connection class.
class iqnet::ssl::Connection: public iqnet::Connection {
protected:
  ssl::Ctx* ssl_ctx;
  SSL *ssl;
  
public:
  Connection( int sock, const iqnet::Inet_addr& );
  ~Connection();
  
  void shutdown();
  int send( const char*, int );
  int recv( char*, int );

  //! Does ssl_accept()
  void post_accept();
  //! Does ssl_connect()
  void post_connect();

protected:
  //! Performs SSL accepting
  virtual void ssl_accept();
  //! Performs SSL connecting
  virtual void ssl_connect();

  bool shutdown_recved();
  bool shutdown_sent();
};


//! Server-side established SSL-connection based on reactive model
//! (with underlying non-blocking socket).
class iqnet::ssl::Reaction_connection: public ssl::Connection {
  Reactor* reactor;

  enum State { EMPTY, ACCEPTING, READING, WRITING, SHUTDOWN };
  State state;
  
  char* recv_buf;
  const char* send_buf;
  int buf_len;

public:
  Reaction_connection( int, const iqnet::Inet_addr&, Reactor* = 0 );

  //! A trick for supporting generic fabric.
  void set_reactor( Reactor* r )
  {
    reactor = r;
  }
  
  void post_accept();
  void handle_input( bool& );
  void handle_output( bool& );

private:
  void try_send();
  int  try_recv();

protected:
  void ssl_accept();
  //! Returns true if shutdown already performed.
  bool reg_shutdown();
  void reg_accept();
  void reg_send( const char*, int );
  void reg_recv( char*, int );

  virtual void accept_succeed() = 0;
  virtual void recv_succeed( bool& terminate, int req_len, int real_len ) = 0;
  virtual void send_succeed( bool& terminate ) = 0;
};


#endif