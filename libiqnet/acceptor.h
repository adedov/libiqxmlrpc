#ifndef _libiqnet_acceptor_h_
#define _libiqnet_acceptor_h_

#include <string>
#include <libiqnet/inet_addr.h>
#include <libiqnet/reactor.h>

namespace iqnet 
{
  class Accepted_conn_fabric;
  class Connection;
  class Acceptor;
};

//! An implementation of pattern that separates TCP-connection
//! establishment from connection handling.
/*!
    Acceptor uses creates server-side socket on port 'port' 
    and waits for incoming connections. When incoming connection 
    is occured the Acceptor is using instance of Connection_fabric 
    to create a specific connection handler.
*/
class iqnet::Acceptor: public iqnet::Event_handler {
  int sock;
  Accepted_conn_fabric *fabric;
  Reactor *reactor;
    
public:
  Acceptor( int port, Accepted_conn_fabric*, Reactor* );
  virtual ~Acceptor();

  const iqnet::Inet_addr get_addr_listening() const;
  void handle_input( bool& );

protected:
  void finish() {}
  int get_fd() const { return sock; }
  
  void accept();
  void listen();
};

#endif
