#pragma once

#include <functional>
#include <map>
#include <string>

#include <castel/runtime/Box.hh>
#include <castel/toolchain/Compiler.hh>
#include <castel/toolchain/Runner.hh>
#include <castel/toolchain/Source.hh>
#include <llvm/IR/Module.h>

class Evaluator
{

private:

    static std::map< std::string, std::function< castel::runtime::Box * ( void ) > > const & standardGlobals( void );

public:

    Evaluator( void );

public:

    inline castel::toolchain::Compiler const & compiler( void ) const;

    inline castel::toolchain::Compiler & compiler( void );

public:

    inline castel::toolchain::Runner const & runner( void ) const;

    inline castel::toolchain::Runner & runner( void );

public:

    llvm::Module * compile( castel::toolchain::Source const & source );

    castel::runtime::Box * run( castel::toolchain::Source const & source );

private:

    castel::toolchain::Compiler mCompiler;

    castel::toolchain::Runner mRunner;

};

castel::toolchain::Compiler const & Evaluator::compiler( void ) const
{
    return mCompiler;
}

castel::toolchain::Compiler & Evaluator::compiler( void )
{
    return mCompiler;
}

castel::toolchain::Runner const & Evaluator::runner( void ) const
{
    return mRunner;
}

castel::toolchain::Runner & Evaluator::runner( void )
{
    return mRunner;
}
