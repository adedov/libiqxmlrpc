#include <iostream>
#include <vector>
#include <libiqxmlrpc/value.h>

using namespace iqxmlrpc;


int main()
{
  try
  {
    {
    Value vi = 10;
    Value vd = 0.33;
    Value vs = "Test String";

    std::cout << "Test Value's type conversion:" << std::endl;
    int i = vi;
    std::cout << "vi=" << i << std::endl;
    double d = vd;
    std::cout << "vd=" << d << std::endl;
    std::string s = vs;
    std::cout << "vs=" << s << std::endl << std::endl;
      
    vs = vd;
    std::cout << "vs=vd; vs=" << vs.get_double() << std::endl << std::endl;
    }
    
    std::cout << "Array testing:" << std::endl;
    Array a;
    a.push_back( 1 );
    a.push_back( 2 );
    a.push_back( 3 ); //v[2]
    
    Value v(a);
    std::cout << "type of v[0]: " << v[0].type_debug() << std::endl;
    
    for( int i = 0; i < v.size(); i++ )
      std::cout << "v[" << i << "]=" << v[i].get_int() << std::endl;
    
    v[2] = "Hello";
    std::cout << "v[2]=" << v[2].get_string() << std::endl << std::endl;
    
    std::vector<int> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    
    std::cout << "assign():" << std::endl;
    a.assign( vec.begin(), vec.end() );
    for( int i = 0; i < a.size(); i++ )
      std::cout << "a[" << i << "]=" << a[i].get_int() << std::endl;
    std::cout << std::endl;    
    
    std::cout << "Struct testing:" << std::endl;
    Struct s;
    s.insert( "author", "D.D.Salinger" );
    s.insert( "title", "The catcher in the rye." );
    s.insert( "pages", 250 );
  
    std::cout 
      << "s {" << std::endl
      << "  author = " << s["author"].get_string() << std::endl
      << "  title  = " << s["title"].get_string()  << std::endl
      << "  pages  = " << s["pages"].get_int()     << std::endl 
      << "}" << std::endl << std::endl;
    
    std::cout << "v[2] = s" << std::endl;
    v[2] = s;
    std::cout << "v[2] = \"a\"" << std::endl << std::endl;
    v[2] = "a";
    
    std::cout << "Testing for incorrect usage cases:" << std::endl;
    try {
      std::cout << "Assigning to v[8]" << std::endl;
      v[8] = "incorrect";
    }
    catch( const std::exception& e )
    {
      std::cout << e.what() << std::endl;
    }
  }
  catch( std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
