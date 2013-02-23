#pragma once

#include <functional>
#include <map>
#include <string>

#include <castel/runtime/Box.hh>
#include <castel/toolchain/Source.hh>

class Program
{

private:

    static std::map< std::string, std::function< castel::runtime::Box * ( void ) > > const & standardGlobals( void );

private:

    castel::runtime::Box * execute( castel::toolchain::Source source );

public:

    int run( int argc, char ** argv );

};
