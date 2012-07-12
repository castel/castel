#include <iostream>
#include <string>

#include <llvm/Function.h>
#include <p9/lexer/Lexer.hh>
#include <p9/parser/Parser.hh>

#include "CompilerVisitor.hh"

int main( void )
{
    std::istream & istream = std::cin;

    std::istreambuf_iterator< char > eos;
    std::string s( std::istreambuf_iterator< char >( istream ), eos );

    p9::lexer::Lexer lexer( s.c_str( ), s.length( ) );
    p9::parser::Parser parser( lexer );
    p9::ast::Token * ast = parser.exec( );

    CompilerVisitor compilerVisitor;
    llvm::Value * value = compilerVisitor.codegen( *ast );
}
