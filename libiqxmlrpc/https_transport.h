#ifndef _libiqxmlrpc_https_transport_h_
#define _libiqxmlrpc_https_transport_h_

#include <libiqxmlrpc/http.h>
#include <libiqnet/acceptor.h>
#include <libiqnet/ssl_connection.h>
#include <libiqnet/conn_fabric.h>
#include <libiqnet/connector.h>

namespace iqxmlrpc
{
  class Https_reaction_connection;
  class Https_conn_fabric;
  class Https_server;
  class Https_client;
};


//! Represents server-side \b HTTPS non-blocking connection.
/*! Does the actual work for sending/receiving HTTP packets via SSL.
    \see iqxmlrpc::Https_server
*/
class iqxmlrpc::Https_reaction_connection: 
  public iqnet::ssl::Reaction_connection 
{
  http::Server* server;
  http::Packet* response;

  const int recv_buf_sz;
  char* recv_buf;
  char* send_buf;
  
  friend class Https_conn_fabric;
    
public:
  Https_reaction_connection( int, const iqnet::Inet_addr& );
  ~Https_reaction_connection();

  void finish() { delete this; }

protected:
  void my_reg_recv();
  void accept_succeed();
  void recv_succeed( bool& terminate, int req_len, int real_len );
  void send_succeed( bool& terminate );
};


//! Fabric for Https_reaction_connection.
class iqxmlrpc::Https_conn_fabric: 
  public iqnet::Serial_conn_fabric<Https_reaction_connection> 
{
  Method_dispatcher *disp;
  iqnet::Reactor* reactor;

public:
  Https_conn_fabric( Method_dispatcher* d, iqnet::Reactor* r ):
    disp(d), reactor(r) {}
  
  void post_create( Https_reaction_connection* c )
  {
    c->set_reactor( reactor );
    c->server = new http::Server( disp );
  }
};


//! Single thread XML-RPC \b HTTPS server based on reactive model.
/*! It just accepts new connections and creates objects of
    Https_reaction_connection class for each one. 
    Then Https_reaction_connection does all real work. */
class iqxmlrpc::Https_server {
  typedef Https_conn_fabric C_fabric;
  
  iqnet::Reactor   reactor;
  iqnet::Acceptor *acceptor;
  C_fabric        *cfabric;

  bool  exit_flag;

public:
  Https_server( int port, Method_dispatcher* );
  ~Https_server();

  void set_exit_flag()
  {
    exit_flag = true;
  }
  
  void work();
};


//! Single thread XML-RPC \b HTTPS client based on blocking connection.
/*! Implements functions for real network collaboration,
    sending/receiving HTTPS packets using SSL.
*/
class iqxmlrpc::Https_client: public iqxmlrpc::http::Client {
  iqnet::Inet_addr addr;
  iqnet::ssl::Connection* conn;
  iqnet::Connector<iqnet::ssl::Connection> ctr;
  
public:
  Https_client( const iqnet::Inet_addr&, const std::string& uri="/RPC" );
  virtual ~Https_client();

protected:
  void send_request( const http::Packet& );
  void recv_response();
};

#endif
