#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "OptionBag.hh"

OptionBag::OptionBag( int argc, char ** argv )
{
    namespace po = boost::program_options;

    po::options_description description( "Allowed options" );
    description.add_options( )
        ( "input-file",   po::value< std::string >( ),                "Specifies program source file" )
        ( "arguments",    po::value< std::vector< std::string > >( ), "Arguments passed to program" )
        ( "check-syntax",                                             "Only checks input syntax" )
        ( "emit-llvm",                                                "Produces LLVM IR output instead of interpreting input" )
        ( "help",                                                     "Produces help message" )
    ;

    po::positional_options_description positionalDescription;
    positionalDescription.add( "input-file", 1 );
    positionalDescription.add( "arguments", - 1 );

    po::variables_map variables;
    po::store( po::command_line_parser( argc, argv ).options( description ).positional( positionalDescription ).run( ), variables );
    po::notify( variables );

    mCheckSyntax = variables.count( "check-syntax" ) > 0;
    mEmitLLVM = variables.count( "emit-llvm" ) > 0;
    mHelp = variables.count( "help" ) > 0;

    if ( variables.count( "input-file" ) )
        mInputFile = variables[ "input-file" ].as< std::string >( );

    if ( variables.count( "arguments" ) )
        mArguments = std::move( variables[ "arguments" ].as< std::vector< std::string > >( ) );

    std::ostringstream usageStream;
    description.print( usageStream );
    mUsageString = usageStream.str( );
}
