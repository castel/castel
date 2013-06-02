#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <castel/ast/Statement.hh>
#include <castel/lex/Exception.hh>
#include <castel/parse/Exception.hh>
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
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <readline/readline.h>

#include "Program.hh"
#include "REPL.hh"

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

castel::toolchain::Source Program::makeSource( std::string const & from ) const
{
    if ( from == "-" ) {
        return castel::toolchain::Source::fromStream( std::cin );
    } else {
        return castel::toolchain::Source::fromFile( from );
    }
}

std::string Program::formatBox( castel::runtime::Box * box ) const
{
    std::ostringstream ss;

    if ( dynamic_cast< castel::runtime::boxes::Class * >( box ) )
        ss << "<Class>";

    if ( dynamic_cast< castel::runtime::boxes::Function * >( box ) )
        ss << "<Function>";

    if ( dynamic_cast< castel::runtime::boxes::Number * >( box ) )
        ss << "<Number : " << static_cast< castel::runtime::boxes::Number * >( box )->value( ) << ">";

    if ( dynamic_cast< castel::runtime::boxes::Boolean * >( box ) )
        ss << "<Boolean : " << static_cast< castel::runtime::boxes::Boolean * >( box )->value( ) << ">";

    if ( dynamic_cast< castel::runtime::boxes::String * >( box ) )
        ss << "<String : " << static_cast< castel::runtime::boxes::String * >( box )->value( ) << ">";

    if ( dynamic_cast< castel::runtime::boxes::Null * >( box ) )
        ss << "<Null>";

    if ( dynamic_cast< castel::runtime::boxes::Undefined * >( box ) )
        ss << "<Undefined>";

    if ( dynamic_cast< castel::runtime::boxes::Dict * >( box ) )
        ss << "<Dict>";

    if ( dynamic_cast< castel::runtime::boxes::List * >( box ) )
        ss << "<List>";

    if ( dynamic_cast< castel::runtime::boxes::Object * >( box ) )
        ss << "<Instance>";

    return ss.str( );
}

int Program::help( void ) const
{
    std::cerr << mOptionBag.usageString( );

    return 1;
}

int Program::checkSyntax( void ) const
{
    std::cerr << "Not yet implemented." << std::endl;

    return 1;
}

int Program::emitLLVM( void ) const
{
    /*
    if ( mOptionBag.inputFile( ) == "" )
        return this->help( );

    castel::toolchain::Compiler compiler;
    for ( auto & it : Program::standardGlobals( ) )
        compiler.globals( ).push_back( it.first );

    llvm::Module * module;
    try { module = compiler.build( this->makeSource( mOptionBag.inputFile( ) ).parse( ) );
    } catch ( ... ) { throw ; }

    llvm::PassManager passManager;
    passManager.add( llvm::createPrintModulePass( & llvm::outs( ) ) );
    passManager.run( * module );
    */

    return 0;
}

int Program::interpret( void ) const
{
    /*
    castel::ast::Statement * statements = this->makeSource( mOptionBag.inputFile( ) ).parse( );

    castel::toolchain::Compiler compiler;
    for ( auto & it : Program::standardGlobals( ) )
        compiler.globals( ).push_back( it.first );

    llvm::Module * module = compiler.build( this->makeSource( mOptionBag.inputFile( ) ).parse( ) );

    castel::toolchain::Runner runner( module );
    for ( auto & it : Program::standardGlobals( ) )
        runner.globals( )[ it.first ] = it.second;

    castel::runtime::Box * box;
    try { box = runner( ); }
    catch ( ... ) { throw ; }

    if ( dynamic_cast< castel::runtime::boxes::Undefined * >( box ) == nullptr )
        std::cout << "Program returned " << this->formatBox( box ) << std::endl;
    */

    return 0;
}

int Program::run( void ) const
{
    if ( mOptionBag.help( ) ) {
        return this->help( );

    } else if ( mOptionBag.checkSyntax( ) ) {
        return this->checkSyntax( );

    } else if ( mOptionBag.emitLLVM( ) ) {
        return this->emitLLVM( );

    } else if ( mOptionBag.inputFile( ) != "" ) {
        return this->interpret( );

    } else {
        return REPL( ).run( );

    }
}
