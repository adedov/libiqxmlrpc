/*! \file libiqxmlrpc.h */
// This file contains #include macroses and Doxygen docs only.

#ifndef _libiqxmlrpc_h_
#define _libiqxmlrpc_h_

#include <libiqxmlrpc/except.h>
#include <libiqxmlrpc/value.h>
#include <libiqxmlrpc/response.h>
#include <libiqxmlrpc/method.h>
#include <libiqxmlrpc/http.h>

/*!
\mainpage libiqxmlrpc Documentation

\section guide_content Users guide
- \ref about
- \ref features

- <b>Data representation</b>
  - \ref value_types
  - \ref value_usage

- \ref exceptions

- <b>Writing XML-RPC server</b>
  - \ref define_methods
  - \ref network_server
*/

/*! 
\page about About
This library provides Object Oriented API for 
<a href="http://www.xmlrpc.com>XML-RPC</a> protocol.

Main goals of project is to provide:
  -# Nice object oriented design;
  -# Maximum separation of code which operates with XML-RPC structures
     from that one, which provides transport functionality;
  -# HTTP, HTTPS as standart transports;
  -# Comfortable way to extend library's functionality.

\author Anton Dedov <adedov@iqmedia.com> for IQ Media, Inc.
*/

/*!
\page features Features
\todo Features documentation
*/

#endif
