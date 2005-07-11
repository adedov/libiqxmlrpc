#include "libiqxmlrpc/util.h"

using namespace iqxmlrpc::util;

LockedBool::LockedBool(bool def, iqnet::Lock* l):
  val(def),
  lock(l)
{
}

LockedBool::~LockedBool()
{
}

LockedBool::operator bool()
{
  iqnet::Auto_lock alock(lock.get());
  return val;
}

void LockedBool::operator =(bool b)
{
  iqnet::Auto_lock alock(lock.get());
  val = b;
}
