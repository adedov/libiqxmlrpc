//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2013 Anton Dedov

#ifndef _iqxmlrpc_value_type_inl_
#define _iqxmlrpc_value_type_inl_

#include "value.h"

namespace iqxmlrpc {

template <class In>
void Array::assign( In first, In last )
{
  clear();
  for( ; first != last; ++first )
    values.push_back( new Value(*first) );
}

} // namespace iqxmlrpc

#endif
