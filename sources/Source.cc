#include <castel/lex/Lexer.hh>
#include <castel/parse/Parser.hh>

#include "Source.hh"

castel::ast::Statement * Source::parse( void ) const
{
    castel::lex::Lexer lexer( mCode.c_str( ), mCode.length( ) );
    castel::parse::Parser parser( lexer );
    return parser.exec( );
}
