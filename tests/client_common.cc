#include "client_common.h"

using namespace iqxmlrpc;

Method_proxy::Method_proxy(iqxmlrpc::Client_base* cb, const std::string& name):
  client_(cb), method_name_(name)
{
}

Method_proxy::~Method_proxy()
{
}

Response Method_proxy::operator ()(const iqxmlrpc::Value& val)
{
  Param_list pl;
  pl.push_back(val);
  return client_->execute(method_name_, pl);
}

Response Method_proxy::operator ()(const iqxmlrpc::Value& val, const TraceInfo& trace_info) {
  Param_list pl;
  pl.push_back(val);
  return client_->execute(method_name_, pl, trace_info); 
}
