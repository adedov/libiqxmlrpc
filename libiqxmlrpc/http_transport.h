#ifndef _libiqxmlrpc_http_transport_h_
#define _libiqxmlrpc_http_transport_h_

#include <libiqxmlrpc/http.h>
#include <libiqnet/acceptor.h>
#include <libiqnet/connection.h>
#include <libiqnet/conn_fabric.h>
#include <libiqnet/connector.h>

namespace iqxmlrpc
{
  template <class Conn_type> class Reaction_conn_fabric;
  class Http_reaction_connection;

  class Http_server;
  class Http_client;
};


class iqxmlrpc::Http_reaction_connection: 
  public iqnet::Connection, 
  private http::Server 
{
private:
  iqnet::Reactor* reactor;
  http::Packet* response;
  
public:
  Http_reaction_connection( 
    int, 
    const iqnet::Inet_addr&, 
    iqnet::Reactor*, 
    Method_dispatcher* 
  );

  ~Http_reaction_connection();

  void post_init();  
  void finish();
  
  void handle_input( bool& );
  void handle_output( bool& );
};


template <class Conn_type>
class iqxmlrpc::Reaction_conn_fabric: public iqnet::Connection_fabric {
  Method_dispatcher *disp;
  iqnet::Reactor* reactor;

public:
  Reaction_conn_fabric( Method_dispatcher* d, iqnet::Reactor* r ):
    disp(d), reactor(r) {}
  
  void create_accepted( int fd, const iqnet::Inet_addr& addr )
  {
    Conn_type *c = new Conn_type( fd, addr, reactor, disp );
    c->post_init();
  }
};


class iqxmlrpc::Http_server {
  typedef Reaction_conn_fabric<Http_reaction_connection> C_fabric;
  
  iqnet::Reactor   reactor;
  iqnet::Acceptor *acceptor;
  C_fabric        *cfabric;

  bool  exit_flag;

public:
  Http_server( int port, Method_dispatcher* );
  ~Http_server();

  void set_exit_flag()
  {
    exit_flag = true;
  }
  
  void work();
};


class iqxmlrpc::Http_client: public iqxmlrpc::http::Client {
  iqnet::Inet_addr addr;
  iqnet::Connection* conn;
  iqnet::Connector<iqnet::Connection> ctr;
  
public:
  Http_client( const iqnet::Inet_addr&, const std::string& uri="/RPC" );
  virtual ~Http_client();

protected:
  void send_request( const http::Packet& );
  void recv_response();
};


#endif
