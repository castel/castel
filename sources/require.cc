#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/operations.hpp>
#include <castel/runtime/Box.hh>
#include <castel/toolchain/Source.hh>

#include "Evaluator.hh"
#include "require.hh"

static std::string getPath( void )
{
    char const * env = std::getenv( "CASTEL_PATH" );
    if ( env ) return env;

    return "";
}

castel::runtime::Box * require( std::string const & module ) {

    std::vector< std::string > pathFolders;

    std::string path = getPath( );
    boost::split( pathFolders, path, boost::is_any_of( ":" ) );

    for ( auto const & folder : pathFolders ) {

        if ( folder.size( ) == 0 )
            continue ;

        std::string filePath = path + "/" + module + ".ct";

        if ( ! boost::filesystem::exists( filePath ) )
            continue ;

        std::ifstream stream( filePath );

        if ( stream.bad( ) )
            throw std::runtime_error( "I/O error while loading module '" + module + "'" );

        return Evaluator( ).run( castel::toolchain::Source::fromStream( stream ) );

    }

    throw std::runtime_error( "Module '" + module + "' not found" );
}
