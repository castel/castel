#pragma once

#include <list>
#include <map>

#include <llvm/LLVMContext.h>

#include "Runner.hh"

class Source;

class Compiler
{

public:

    Compiler( void );

public:

    inline std::list< std::string > const & globals( void ) const;

    inline std::list< std::string > & globals( void );

    inline Compiler & globals( std::list< std::string > const & globals );

public:

    Runner build( Source const & source );

private:

    std::list< std::string > mGlobals;

    llvm::LLVMContext mContext;

};

#include "Source.hh"

std::list< std::string > const & Compiler::globals( void ) const
{
    return mGlobals;
}

std::list< std::string > & Compiler::globals( void )
{
    return mGlobals;
}

Compiler & Compiler::globals( std::list< std::string > const & globals )
{
    mGlobals = globals;

    return * this;
}
