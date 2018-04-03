#include "xheaders.h"

#include <boost/algorithm/string.hpp>

namespace iqxmlrpc
{
XHeaders& XHeaders::operator=(const std::map<std::string, std::string>& v) {
  xheaders_.clear();
  for (XHeaders::const_iterator it = v.begin(); it != v.end(); ++it) {
    if (validate(it->first)) {
      std::string key(it->first);
      boost::to_lower(key);
      xheaders_[key] = it->second;
    }
  }
  return *this;
}

XHeaders::~XHeaders() {}

std::string& XHeaders::operator[] (const std::string& v) {
  if (!validate(v)) {
    throw Error_xheader("The header doesn't starts with `X-`");
  }
  std::string key(v);
  boost::to_lower(key);
  return xheaders_[key];
}

XHeaders::const_iterator XHeaders::find (const std::string& k) const {
  std::string key(k);
  boost::to_lower(key);
  return xheaders_.find(key);
}

XHeaders::const_iterator XHeaders::begin() const {
  return xheaders_.begin();
}

XHeaders::const_iterator XHeaders::end() const {
  return xheaders_.end();
}

bool XHeaders::validate(const std::string& val) {
  return boost::starts_with(val, "X-") || boost::starts_with(val, "x-");
}

Error_xheader::Error_xheader(const char* msg) : invalid_argument(msg) {}
}
