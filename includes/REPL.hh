#pragma once

#include <set>
#include <string>

#include <castel/ast/tools/List.hh>
#include <castel/ast/Statement.hh>
#include <castel/runtime/Box.hh>
#include <castel/toolchain/Compiler.hh>
#include <castel/toolchain/Runner.hh>

class REPL
{

public:

    REPL( void );

public:

    int run( void );

private:

    castel::ast::tools::List< castel::ast::Statement > parse( char * line );

    void wrap( castel::ast::tools::List< castel::ast::Statement > & statements );

    castel::runtime::Box * execute( castel::ast::tools::List< castel::ast::Statement > const & statement );

    std::string format( castel::runtime::Box * box );

    void print( castel::runtime::Box * box );

private:

    castel::runtime::Box * mLastContext;

    std::set< std::string > mSymbols;

    castel::toolchain::Compiler mCompiler;

    castel::toolchain::Runner mRunner;

};
