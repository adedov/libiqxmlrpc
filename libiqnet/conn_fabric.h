#ifndef _libiqnet_connection_fabric_
#define _libiqnet_connection_fabric_

#include <string>
#include <libiqnet/inet_addr.h>

namespace iqnet 
{
  class Accepted_conn_fabric;
    
  template<class Conn_type> 
  class Serial_conn_fabric;
};


//! Abstract fabric for accepted connections.
class iqnet::Accepted_conn_fabric {
public:
  virtual ~Accepted_conn_fabric() {}
  virtual void create_accepted( int sock, const Inet_addr& peer_addr ) = 0;
};


//! Fabric for single threaded connections.
template <class Conn_type>
class iqnet::Serial_conn_fabric: public iqnet::Accepted_conn_fabric {
public:
  void create_accepted( int sock, const Inet_addr& peer_addr )
  {
    Conn_type* c = new Conn_type( sock, peer_addr );
    post_create( c );
    c->post_accept();
  }
  
  virtual void post_create( Conn_type* ) {}
};

#endif
