#pragma once

#include <set>
#include <string>

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

    castel::ast::Statement * parse( char * line );

    castel::ast::Statement * wrap( castel::ast::Statement * statements );

    castel::runtime::Box * execute( castel::ast::Statement * statement );

    std::string format( castel::runtime::Box * box );

    void print( castel::runtime::Box * box );

private:

    castel::runtime::Box * mLastContext;

    std::set< std::string > mSymbols;

    castel::toolchain::Compiler mCompiler;

    castel::toolchain::Runner mRunner;

};
