#include <libiqxmlrpc/method.h>

using namespace iqxmlrpc;


Method_dispatcher::~Method_dispatcher()
{
  for( Factory_map::const_iterator i = fs.begin(); i != fs.end(); ++i )
    delete i->second;
}


void Method_dispatcher::register_method
  ( const std::string& name, Method_factory_base* fb )
{
  fs[name] = fb;
}


Method* Method_dispatcher::create_method( const std::string& name )
{
  if( fs.find(name) == fs.end() )
    throw Unknown_method( name );

  Method* m = fs[name]->create();
  m->name_ = name;
  return m;
}
