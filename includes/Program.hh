#include <string>

#include <castel/ast/Statement.hh>
#include <castel/builder/Context.hh>
#include <castel/runtime/Box.hh>
#include <llvm/Function.h>

#pragma once

class Program
{

public:

    int run( int argc, char ** argv );

private:

    std::string fetch( std::string const & source );

    castel::ast::Statement * parse( std::string const & code );

    llvm::Function * build( castel::builder::Context & context, castel::ast::Statement * statements );

    castel::runtime::Box * call( castel::builder::Context & context, llvm::Function * llvmProgram );

};
