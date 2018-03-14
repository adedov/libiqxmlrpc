#pragma once

#include <string>

namespace iqxmlrpc
{
class TraceInfo {
public:
  std::string correlationID;
  std::string spanID;

public:
  TraceInfo(const std::string& correlationID, const std::string& spanID) : 
    correlationID(correlationID),
    spanID(spanID)
  {}
  void operator()(const std::string& cID, const std::string& sID) {
    correlationID = cID;
    spanID = sID;
  }
  TraceInfo() {}
};

inline std::ostream& operator<<(std::ostream& os, const TraceInfo& traceInfo) {
  if( traceInfo.correlationID.length() )
    os << " CorrelationID[" << traceInfo.correlationID << "]";
  if( traceInfo.spanID.length() )
    os << " SpanID[" << traceInfo.spanID << "]";
  return os;
}
}
