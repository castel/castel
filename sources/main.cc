#include <iostream>
#include <stdexcept>
#include <string>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Value.h>
#include <p9/ast/expr/Call.hh>
#include <p9/ast/expr/Function.hh>
#include <p9/ast/expr/Number.hh>
#include <p9/ast/stmt/If.hh>
#include <p9/ast/stmt/Return.hh>
#include <p9/ast/Expression.hh>
#include <p9/ast/Statement.hh>
#include <p9/lexer/Lexer.hh>
#include <p9/parser/Parser.hh>
#include <p9/engine/CodeGenerator.hh>

int main( void )
{
    std::istream & istream = std::cin;

    std::istreambuf_iterator< char > eos;
    std::string s( std::istreambuf_iterator< char >( istream ), eos );

    p9::lexer::Lexer lexer( s.c_str( ), s.length( ) );
    p9::parser::Parser parser( lexer );
    p9::ast::Statement * ast = parser.exec( );

    p9::ast::expr::Function * wrapper = new p9::ast::expr::Function( nullptr, ast );

    llvm::LLVMContext context;
    llvm::IRBuilder< > builder( context );
    llvm::Module module( "main", context );

    p9::engine::CodeGenerator codeGenerator( context, builder, module );
    llvm::Value * programValue = codeGenerator.codegen( *wrapper );
    llvm::Function * program = static_cast< llvm::Function * >( programValue );

    std::string errString;
    llvm::InitializeNativeTarget();
    llvm::ExecutionEngine * engine = llvm::EngineBuilder( &module ).setErrorStr( &errString ).create( );

    if ( ! engine )
        throw std::runtime_error( errString );

    void * fPtr = engine->getPointerToFunction( program );
    double (*callableFPtr)( void ) = reinterpret_cast< double (*)( void ) >( fPtr );
    std::cout << callableFPtr( ) << std::endl;

    return 0;
}
