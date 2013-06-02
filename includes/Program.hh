#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include <castel/runtime/Box.hh>
#include <castel/toolchain/Runner.hh>
#include <castel/toolchain/Source.hh>
#include <llvm/Module.h>

#include "OptionBag.hh"

class Program
{

public:

    inline Program( int argc, char ** argv );

private:

    static std::map< std::string, std::function< castel::runtime::Box * ( void ) > > const & standardGlobals( void );

private:

    castel::toolchain::Source makeSource( std::string const & from ) const;

private:

    std::string formatBox( castel::runtime::Box * box ) const;

private:

    int help( void ) const;

    int checkSyntax( void ) const;

    int emitLLVM( void ) const;

    int interpret( void ) const;

    int repl( void ) const;

public:

    int run( void ) const;

private:

    OptionBag mOptionBag;

};

Program::Program( int argc, char ** argv )
    : mOptionBag( argc, argv )
{
}
