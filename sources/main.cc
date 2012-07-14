#include <iostream>
#include <string>

#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Value.h>
#include <p9/ast/expr/Call.hh>
#include <p9/ast/expr/Function.hh>
#include <p9/ast/Statement.hh>
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
    p9::ast::Statement * ast = parser.exec( );

    p9::ast::expr::Function * fnWrapper = new p9::ast::expr::Function( nullptr, ast );
    p9::ast::expr::Call * callWrapper = new p9::ast::expr::Call( fnWrapper, nullptr );

    llvm::LLVMContext context;
    llvm::IRBuilder< > builder( context );
    llvm::Module module( "main", context );

    CompilerVisitor compilerVisitor( context, builder );
    callWrapper->accept( compilerVisitor );

    module.dump( );

    return 0;
}
