#ifndef _libiqnet_connection_h_
#define _libiqnet_connection_h_

#include <string>
#include <libiqnet/inet_addr.h>
#include <libiqnet/reactor.h>


namespace iqnet 
{
  class Connection;
}


//! An established TCP-connection.
/*!
    A build block for connection handling. 
    Have to be created by Connection_fabric.
*/
class iqnet::Connection: public iqnet::Event_handler {
protected:
  int sock;
  iqnet::Inet_addr peer_addr;
  std::string cache;

public:
  Connection( int socket_d, const iqnet::Inet_addr& peer_addr );
  virtual ~Connection();

  //! Do some init actions. May be used by child classes.
  virtual void post_init() {}
  
  const iqnet::Inet_addr& get_peer_addr() const { return peer_addr; }
  
  virtual int send( const char*, int );
  virtual int recv( char*, int );

  void send_str( const std::string& );
  const std::string recv_str();  
  
protected:
  int get_fd() const { return sock; }

  void set_non_blocking( bool );
  bool get_non_blocking() const;
};

#endif
