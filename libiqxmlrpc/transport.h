#ifndef _libiqxmlrpc_transport_h_
#define _libiqxmlrpc_transport_h_

#include <string>
#include <libiqxmlrpc/request.h>
#include <libiqxmlrpc/response.h>


namespace iqxmlrpc 
{
  class Method_dispatcher;
  class Server;
  class Client;
};


//! Transport independent XML-RPC server.
class iqxmlrpc::Server {
  Method_dispatcher* disp;
  Request* request;
  Value resp_value;

public:
  Server( Method_dispatcher* );
  virtual ~Server();

  Response execute( const std::string& xmlrpc_request );

private:
  void parse_request( const std::string& );
  void execute();
};


#endif
