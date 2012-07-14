#include <iostream>
#include <string>

#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Value.h>
#include <p9/ast/expr/Call.hh>
#include <p9/ast/expr/Function.hh>
#include <p9/ast/expr/Number.hh>
#include <p9/ast/stmt/Return.hh>
#include <p9/ast/Expression.hh>
#include <p9/ast/Statement.hh>

#include "CompilerVisitor.hh"

int main( void )
{
    /**
    std::istream & istream = std::cin;

    std::istreambuf_iterator< char > eos;
    std::string s( std::istreambuf_iterator< char >( istream ), eos );

    p9::lexer::Lexer lexer( s.c_str( ), s.length( ) );
    p9::parser::Parser parser( lexer );
    p9::ast::Statement * ast = parser.exec( );
    **/

    p9::ast::Expression * returnValue1 = new p9::ast::expr::Number( 42 );
    p9::ast::Statement  * returnStmt1  = new p9::ast::stmt::Return( returnValue1 );
    p9::ast::Expression * function1    = new p9::ast::expr::Function( nullptr, returnStmt1 );

    p9::ast::Expression * returnValue2 = new p9::ast::expr::Call( function1, nullptr );
    p9::ast::Statement  * returnStmt2  = new p9::ast::stmt::Return( returnValue2 );
    p9::ast::Expression * function2    = new p9::ast::expr::Function( nullptr, returnStmt2 );

    llvm::LLVMContext context;
    llvm::IRBuilder< > builder( context );
    llvm::Module module( "main", context );

    CompilerVisitor compilerVisitor( context, builder );
    compilerVisitor.codegen( *function2 );

    module.dump( );

    return 0;
}
