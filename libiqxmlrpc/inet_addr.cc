//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <boost/optional.hpp>
#include "inet_addr.h"
#include "net_except.h"

namespace iqnet {

std::string get_host_name()
{
  char buf[1024];
  buf[1023] = 0;
  ::gethostname( buf, sizeof(buf) );

  return buf;
}


#if defined(_WIN32) || defined(__APPLE__)
#define IQXMLRPC_GETHOSTBYNAME(_h) \
  hent = ::gethostbyname( _h ); \
  if( !hent ) { \
    throw network_error( "gethostbyname" ); \
  }
#endif

#if !defined(IQXMLRPC_GETHOSTBYNAME)
#define IQXMLRPC_GETHOSTBYNAME_PREP \
  struct hostent hent_local; \
  char buf[1024]; \
  int local_h_errno = 0;
#endif

#if !defined(IQXMLRPC_GETHOSTBYNAME)
#define IQXMLRPC_GETHOSTBYNAME_POST \
  if( !hent ) { \
    throw network_error( "gethostbyname: " + std::string(hstrerror(local_h_errno)), false ); \
  }
#endif

#if defined(__sun) || defined(sun)
#define IQXMLRPC_GETHOSTBYNAME(_h) \
  IQXMLRPC_GETHOSTBYNAME_PREP \
  hent = ::gethostbyname_r( _h), &hent_local, buf, sizeof(buf), &local_h_errno ); \
  IQXMLRPC_GETHOSTBYNAME_POST
#endif

#if !defined(IQXMLRPC_GETHOSTBYNAME)
#define IQXMLRPC_GETHOSTBYNAME(_h) \
  IQXMLRPC_GETHOSTBYNAME_PREP \
  ::gethostbyname_r( _h, &hent_local, buf, sizeof(buf), &hent, &local_h_errno ); \
  IQXMLRPC_GETHOSTBYNAME_POST
#endif

typedef struct sockaddr_in SystemSockAddrIn;

struct Inet_addr::Impl {
  mutable boost::optional<SystemSockAddrIn> sa;
  std::string host;
  int port;

  Impl( const SystemSockAddrIn& );
  Impl( const std::string& host, int port );
  Impl( int port );

  void init_sockaddr() const;
};

Inet_addr::Impl::Impl( const std::string& h, int p ):
  sa(), host(h), port(p)
{
  if (h.find_first_of("\n\r") != std::string::npos)
    throw network_error("Hostname must not contain CR LF characters", false);
}

Inet_addr::Impl::Impl( int p ):
  sa(SystemSockAddrIn()), host("0.0.0.0"), port(p)
{
  sa->sin_family = PF_INET;
  sa->sin_port = htons(port);
  sa->sin_addr.s_addr = INADDR_ANY;
}

Inet_addr::Impl::Impl( const SystemSockAddrIn& s ):
  sa(s)
{
  host = inet_ntoa( sa->sin_addr );
  port = ntohs( sa->sin_port );
}

void
Inet_addr::Impl::init_sockaddr() const
{
  sa = SystemSockAddrIn();
  struct hostent* hent = 0;
  IQXMLRPC_GETHOSTBYNAME(host.c_str());
  sa->sin_family = PF_INET;
  sa->sin_port = htons(port);
  memcpy( (char*)&(sa->sin_addr), (char*)hent->h_addr, hent->h_length );
}

Inet_addr::Inet_addr( const std::string& host, int port ):
  impl_(new Impl(host, port))
{
}

Inet_addr::Inet_addr( int port ):
  impl_(new Impl(port))
{
}

Inet_addr::Inet_addr( const SystemSockAddrIn& sa ):
  impl_(new Impl(sa))
{
}

const SystemSockAddrIn*
Inet_addr::get_sockaddr() const
{
  if (!impl_->sa) {
    impl_->init_sockaddr();
  }

  return &impl_->sa.get();
}

const std::string&
Inet_addr::get_host_name() const
{
  return impl_->host;
}

int
Inet_addr::get_port() const
{
  return impl_->port;
}

} // namespace iqnet
