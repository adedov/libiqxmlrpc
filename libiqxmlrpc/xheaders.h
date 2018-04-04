#pragma once

#include "api_export.h"

#include <string>
#include <map>
#include <iostream>
#include <stdexcept>

namespace iqxmlrpc
{
class LIBIQXMLRPC_API XHeaders {
private:
  std::map<std::string, std::string> xheaders_;
public:
  typedef std::map<std::string, std::string>::const_iterator const_iterator;
  virtual XHeaders& operator=(const std::map<std::string, std::string>& v);
  virtual std::string& operator[](const std::string& v);
  virtual size_t size() const;
  virtual const_iterator find(const std::string& k) const;
  virtual const_iterator begin() const;
  virtual const_iterator end() const;
  virtual ~XHeaders();

  static bool validate(const std::string& val);
};

inline std::ostream& operator<<(std::ostream& os, const XHeaders& xheaders) {
  for(XHeaders::const_iterator it = xheaders.begin(); it!= xheaders.end(); ++it) {
    os << " " << it->first << "[" << it->second << "]";
  }
  return os;
}

class LIBIQXMLRPC_API Error_xheader: public std::invalid_argument {
public:
  Error_xheader(const char* msg);
};
}
