#ifndef _libiqnet_connection_h_
#define _libiqnet_connection_h_

#include <string>
#include <libiqnet/inet_addr.h>
#include <libiqnet/reactor.h>
#include <libiqnet/net_except.h>


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
public:
  class send_failed;
  
protected:
  int sock;
  bool allow_part_send;
  iqnet::Inet_addr peer_addr;
  std::string cache;

public:
  Connection( int socket_d, const iqnet::Inet_addr& peer_addr );
  virtual ~Connection();

  virtual void post_accept() {}
  virtual void post_connect() {}
  
  const iqnet::Inet_addr& get_peer_addr() const { return peer_addr; }
  
  /*! \b Can \b not cause SIGPIPE signal. */
  virtual int send( const char*, int );
  /*! \b Can \b not cause SIGPIPE signal. */
  virtual int recv( char*, int );

  void send_str( const std::string& );
  const std::string recv_str();  

  //! Defines whether method send() should throw send_failed execption.
  /*! On \b true the method send() \b should \b not throw exception when
      actual sent data has less size than specified.
      By default it is set to \b false.
  */
  void allow_partial_send( bool val )
  {
    allow_part_send = val;
  }
  
protected:
  int get_fd() const { return sock; }

  void set_non_blocking( bool );
  bool get_non_blocking() const;
};


class iqnet::Connection::send_failed: public iqnet::network_error {
public:
  send_failed():
    network_error( "iqnet::Connection::send: Incomplete send.", false ) {}
};


#endif
