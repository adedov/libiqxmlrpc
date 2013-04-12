//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_socket_h_
#define _libiqnet_socket_h_

#include "inet_addr.h"

namespace iqnet
{

//! Relatively portable socket class.
class LIBIQXMLRPC_API Socket {
public:
#ifdef _WINDOWS
  typedef SOCKET Handler;
#else
  typedef int Handler;
#endif //_WINDOWS

private:
  Handler sock;
  Inet_addr peer;

public:
  //! Creates TCP, reusable socket.
  Socket();
  //! Create object from existing socket handler.
  Socket( Handler, const Inet_addr& );
  //! Destructor. Does not close actual socket.
  virtual ~Socket() {}

  Handler get_handler() const { return sock; }

  void shutdown();
  void close();

  //! \note Does not disable non-blocking mode under UNIX.
  void set_non_blocking( bool );

  /*! \b Can \b not cause SIGPIPE signal. */
  virtual size_t send( const char*, size_t );
  virtual void send_shutdown( const char*, size_t );
  /*! \b Can \b not cause SIGPIPE signal. */
  virtual size_t recv( char*, size_t );

  void   bind( const Inet_addr& addr );
  void   listen( unsigned backlog = 5 );
  Socket accept();
  bool   connect( const iqnet::Inet_addr& );

  //! Returns an inet addr the socket asscociated with.
  Inet_addr get_addr() const;

  //! Returns peer addr of connected or accepted socket.
  const Inet_addr& get_peer_addr() const { return peer; }

  //! Returns last error occured with socket.
  int get_last_error();
};

} // namespace iqnet

#endif
