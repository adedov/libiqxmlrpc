#ifndef _libiqnet_connection_fabric_
#define _libiqnet_connection_fabric_

#include <string>
#include <libiqnet/inet_addr.h>

namespace iqnet 
{
  class Connection_fabric;
      
  template<class Conn_type> 
  class Serial_conn_fabric;
};


//! Interface for creating specific connection's handlers.
class iqnet::Connection_fabric {
public:
  virtual ~Connection_fabric() {}
  
  virtual void create_accepted( int sock, const Inet_addr& peer_addr ) = 0;
};


template <class Conn_type>
class iqnet::Serial_conn_fabric: public iqnet::Connection_fabric {
public:
  void create_accepted( int sock, const Inet_addr& peer_addr )
  {
    Conn_type* c = new Conn_type( sock, peer_addr );
    c->post_accept();
  }
};

#endif
