//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
//
//  $Id: api_export.h,v 1.1 2006-09-07 04:42:31 adedov Exp $

#ifndef _libiqxmlrpc_api_export_
#define _libiqxmlrpc_api_export_

#if defined(_WINDOWS) || defined(__MINGW32__)
  #define LIBIQXMLRPC_DLL
#endif // _WINDOWS || __MINGW32__

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
