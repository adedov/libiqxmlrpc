//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_firewall_h_
#define _libiqxmlrpc_firewall_h_

namespace iqnet {

//! Firewall base class.
/*! Used by Acceptor to find out whether it should
    accept XML-RPC requests from specific IP.
*/
class LIBIQXMLRPC_API Firewall_base {
public:
  virtual ~Firewall_base() {}

  //! Must return bool to grant client to send request.
  virtual bool grant( const iqnet::Inet_addr& ) = 0;

  //! Override this method for custom good-bye message.
  /*! Return empty string for closing connection silently. */
  virtual std::string message()
  {
    return "HTTP/1.0 403 Forbidden\r\n";
  }
};

} // namespace iqnet

#endif
