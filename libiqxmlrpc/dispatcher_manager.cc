//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "dispatcher_manager.h"

#include "builtins.h"

#include <algorithm>
#include <deque>
#include <map>

namespace iqxmlrpc {

//
// Default method dispatcher
//

class Default_method_dispatcher: public Method_dispatcher_base {
  typedef std::map<std::string, Method_factory_base*> Factory_map;
  Factory_map fs;

public:
  ~Default_method_dispatcher();

  void register_method(const std::string& name, Method_factory_base*);

private:
  virtual Method*
  do_create_method(const std::string&);

  virtual void
  do_get_methods_list(Array&) const;
};

Default_method_dispatcher::~Default_method_dispatcher()
{
  util::delete_ptrs( fs.begin(), fs.end(),
    util::Select2nd<Factory_map>());
}

void Default_method_dispatcher::register_method
  ( const std::string& name, Method_factory_base* fb )
{
  fs[name] = fb;
}

Method* Default_method_dispatcher::do_create_method(const std::string& name)
{
  if( fs.find(name) == fs.end() )
    return 0;

  return fs[name]->create();
}

void Default_method_dispatcher::do_get_methods_list(Array& retval) const
{
  for(Factory_map::const_iterator i = fs.begin(); i != fs.end(); ++i)
  {
    retval.push_back(i->first);
  }
}

//
// System method factory
//

template <class T>
class System_method_factory: public Method_factory_base {
  Method_dispatcher_manager* dmgr_;

public:
  System_method_factory(Method_dispatcher_manager* dmgr):
    dmgr_(dmgr)
  {
  }

private:
  T* create()
  {
    return new T(dmgr_);
  }
};

//
// Method dispatcher menager
//

class Method_dispatcher_manager::Impl {
public:
  typedef std::deque<Method_dispatcher_base*> DispatchersSet;
  DispatchersSet dispatchers;
  Default_method_dispatcher* default_disp;

  Impl():
    default_disp(new Default_method_dispatcher)
  {
    dispatchers.push_back(default_disp);
  }

  ~Impl()
  {
    util::delete_ptrs(dispatchers.begin(), dispatchers.end());
  }
};


Method_dispatcher_manager::Method_dispatcher_manager():
  impl_(new Impl)
{
}

Method_dispatcher_manager::~Method_dispatcher_manager()
{
  delete impl_;
}

void Method_dispatcher_manager::register_method(
  const std::string& name, Method_factory_base* mfactory)
{
  impl_->default_disp->register_method(name, mfactory);
}

void Method_dispatcher_manager::push_back(Method_dispatcher_base* mdisp)
{
  impl_->dispatchers.push_back(mdisp);
}

Method* Method_dispatcher_manager::create_method(const Method::Data& mdata)
{
  typedef Impl::DispatchersSet::iterator I;
  for (I i = impl_->dispatchers.begin(); i != impl_->dispatchers.end(); ++i)
  {
    Method* tmp = (*i)->create_method(mdata);
    if (tmp)
      return tmp;
  }

  throw Unknown_method(mdata.method_name);
}

void Method_dispatcher_manager::get_methods_list(Array& retval) const
{
  typedef Impl::DispatchersSet::const_iterator CI;
  for (CI i = impl_->dispatchers.begin(); i != impl_->dispatchers.end(); ++i)
    (*i)->get_methods_list(retval);
}

void Method_dispatcher_manager::enable_introspection()
{
  impl_->default_disp->register_method("system.listMethods",
    new System_method_factory<builtins::List_methods>(this));
}

} // namespace iqxmlrpc
