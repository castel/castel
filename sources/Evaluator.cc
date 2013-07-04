#include <functional>
#include <map>
#include <string>

#include <castel/ast/tools/List.hh>
#include <castel/ast/Statement.hh>
#include <castel/lex/Exception.hh>
#include <castel/parse/Exception.hh>
#include <castel/runtime/helper/memoize.hh>
#include <castel/runtime/helper/wrap.hh>
#include <castel/runtime/interface.hh>
#include <castel/toolchain/Source.hh>
#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/Module.h>

#include "Evaluator.hh"
#include "require.hh"

Evaluator::Evaluator( void )
{
    for ( auto & it : Evaluator::standardGlobals( ) ) {
        mCompiler.globals( ).push_back( it.first );
        mRunner.globals( )[ it.first ] = it.second;
    }
}

std::map< std::string, std::function< castel::runtime::Box * ( void ) > > const & Evaluator::standardGlobals( void )
{
    static std::map< std::string, std::function< castel::runtime::Box * ( void ) > > globals;
    static bool initialized = false;

    if ( initialized ) return globals;
    initialized = true;

    globals[ "Class" ] = castel::runtime::helper::memoize( std::function< castel::runtime::Box * ( void ) >( [ ] ( ) {
        return Castel_Class_initialize( nullptr );
    } ) );

    globals[ "require" ] = castel::runtime::helper::memoize( std::function< castel::runtime::Box * ( void ) >( [ ] ( ) {
        return castel::runtime::helper::wrap( & require );
    } ) );

    return globals;
}

llvm::Module * Evaluator::compile( castel::toolchain::Source const & source )
{
    castel::ast::tools::List< castel::ast::Statement > statements;

    try {
        statements = source.parse( );
    } catch ( castel::lex::Exception const & e ) {
        std::cerr << "Unexpected symbol" << std::endl;
    } catch ( castel::parse::Exception const & e ) {
        std::cerr << "Unexpected token" << std::endl;
    }

    return mCompiler.build( statements );
}

castel::runtime::Box * Evaluator::run( castel::toolchain::Source const & source )
{
    llvm::Module * module = this->compile( source );

    llvm::verifyModule( * module );
    return mRunner.run( module );
}
