//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_api_export_
#define _libiqxmlrpc_api_export_

#include "sysinc.h"

#if defined(_WIN32) || defined(__MINGW32__)
  #define LIBIQXMLRPC_DLL
#endif // _WIN32 || __MINGW32__

#ifdef LIBIQXMLRPC_DLL
  #if defined(LIBIQXMLRPC_COMPILATION) && defined(DLL_EXPORT)
    #define LIBIQXMLRPC_API __declspec(dllexport)
  #elif !defined(LIBIQXMLRPC_COMPILATION)
    #define LIBIQXMLRPC_API __declspec(dllimport)
  #else
    #define LIBIQXMLRPC_API
  #endif // LIBIQXMLRPC_COMPILATION && DLL_EXPORT
#elif defined(__GNUC__) && __GNUC__ > 3
  #define LIBIQXMLRPC_API __attribute__((visibility("default")))
#else
  #define LIBIQXMLRPC_API
#endif // LIBIQXMLRPC_DLL

#endif
