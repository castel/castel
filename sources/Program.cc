#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include <castel/runtime/boxes/Boolean.hh>
#include <castel/runtime/boxes/Class.hh>
#include <castel/runtime/boxes/Dict.hh>
#include <castel/runtime/boxes/Function.hh>
#include <castel/runtime/boxes/List.hh>
#include <castel/runtime/boxes/Null.hh>
#include <castel/runtime/boxes/Number.hh>
#include <castel/runtime/boxes/Object.hh>
#include <castel/runtime/boxes/String.hh>
#include <castel/runtime/boxes/Undefined.hh>
#include <castel/runtime/helper/create.hh>
#include <castel/runtime/helper/memoize.hh>
#include <castel/runtime/helper/wrap.hh>
#include <castel/runtime/Box.hh>
#include <castel/runtime/Module.hh>
#include <castel/runtime/interface.hh>
#include <castel/toolchain/Compiler.hh>
#include <castel/toolchain/Runner.hh>
#include <castel/toolchain/Source.hh>

#include "Program.hh"

std::map< std::string, std::function< castel::runtime::Box * ( void ) > > const & Program::standardGlobals( void )
{
    static std::map< std::string, std::function< castel::runtime::Box * ( void ) > > globals;
    static bool initialized = false;

    if ( initialized ) return globals;
    initialized = true;

    globals[ "Class" ] = castel::runtime::helper::memoize( std::function< castel::runtime::Box * ( void ) >( [ ] ( ) {
        return Castel_Class_initialize( nullptr );
    } ) );

    return globals;
}

castel::runtime::Box * Program::execute( castel::toolchain::Source source )
{
    std::map< std::string, std::function< castel::runtime::Box * ( void ) > > globals( Program::standardGlobals( ) );

    castel::toolchain::Compiler compiler;
    for ( auto & it : globals )
        compiler.globals( ).push_back( it.first );

    castel::toolchain::Runner runner( compiler.build( source ) );
    for ( auto & it : globals )
        runner.globals( )[ it.first ] = it.second;

    try {
        return runner( );
    } catch ( ... ) {
        std::cout << "An uncaught Castel exception has occured !" << std::endl;
        return nullptr;
    }
}

int Program::run( int argc, char ** argv )
{
    castel::runtime::Box * box;

    if ( argc >= 2 )
        box = this->execute( castel::toolchain::Source::fromFile( argv[ 1 ] ) );
    else
        box = this->execute( castel::toolchain::Source::fromStream( std::cin ) );

    if ( dynamic_cast< castel::runtime::boxes::Class * >( box ) )
        std::cout << "Returned a class" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Function * >( box ) )
        std::cout << "Returned a function" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Number * >( box ) )
        std::cout << "Returned a number (" << static_cast< castel::runtime::boxes::Number * >( box )->value( ) << ")" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Boolean * >( box ) )
        std::cout << "Returned a boolean (" << static_cast< castel::runtime::boxes::Boolean * >( box )->value( ) << ")" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::String * >( box ) )
        std::cout << "Return a string (" << static_cast< castel::runtime::boxes::String * >( box )->value( ) << ")" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Null * >( box ) )
        std::cout << "Returned a null value" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Undefined * >( box ) )
        std::cout << "Returned an undefined value" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Dict * >( box ) )
        std::cout << "Returned a dict" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::List * >( box ) )
        std::cout << "Returned a list" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Object * >( box ) )
        std::cout << "Returned an object" << std::endl;

    return 0;
}
