#pragma once

#include <functional>
#include <map>

#include <castel/runtime/Box.hh>
#include <llvm/Module.h>

class Runner
{

private:

    static void staticInitializer( void );

private:

    static castel::runtime::Box * staticDependencyInitializer( Runner * runner, char const * global );

public:

    Runner( llvm::Module * module );

public:

    inline std::map< std::string, std::function< castel::runtime::Box * ( void ) > > const & globals( void ) const;

    inline std::map< std::string, std::function< castel::runtime::Box * ( void ) > > & globals( void );

public:

    castel::runtime::Box * operator()( void );

private:

    llvm::Module * mModule;

    std::map< std::string, std::function< castel::runtime::Box * ( void ) > > mGlobals;

};

std::map< std::string, std::function< castel::runtime::Box * ( void ) > > const & Runner::globals( void ) const
{
    return mGlobals;
}

std::map< std::string, std::function< castel::runtime::Box * ( void ) > > & Runner::globals( void )
{
    return mGlobals;
}
