#ifndef _iqxmlrpc_server_h_
#define _iqxmlrpc_server_h_

#include <string>
#include <map>
#include <libiqxmlrpc/value.h>


namespace iqxmlrpc
{
  typedef std::vector<Value> Param_list;

  class Method;
  class Method_factory_base;
  template <class T> class Method_factory;
  class Method_dispatcher;
};


//! Abstract base for server method. 
//! Inherit it to create actual server method.
class iqxmlrpc::Method {
public:
  virtual ~Method() {}

  //! Replace it with your actual code.
  virtual void execute( const Param_list& params, Value& response ) = 0;
};


//! Abstract factory for Method. 
/*! Method_dispatcher uses it to create Method object on demand. 
    Inherit it to create your specific factory.
    \see Method_factory
*/
class iqxmlrpc::Method_factory_base {
public:
  virtual ~Method_factory_base() {}
    
  virtual Method* create() = 0;
};
  

//! Template for simple Method factory.
/*!
Usage:
  \code
  Method_factory_base* myfactory = new Method_factory<MyMethod>;
  \endcode
*/
template <class T>
class iqxmlrpc::Method_factory: public Method_factory_base {
public:
  T* create() { return new T(); }
};


//! Method dispatcher.
/*! This class responsible for methods dispatching by their names.
    User must register his own methods in Method_dispatcher.
    \code
    // Usage example:
    class MyMethod;
    Method_dispatcher disp;
    disp.register_method( "my_method", new Method_factory<MyMethod> );
    \endcode
*/
class iqxmlrpc::Method_dispatcher {
  typedef std::map<std::string, Method_factory_base*> Factory_map;
  Factory_map fs;
  
public:
  virtual ~Method_dispatcher();

  //! Register Method with it fabric. 
  /*! Method_dispatcher responsible for fabric deletion. */
  void register_method( const std::string& name, Method_factory_base* );

  //! Create method object according to specified name. 
  //! Returns 0 if no Method registered for name.
  Method* create_method( const std::string& name );
};


#endif
