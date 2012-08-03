#include <iostream>
#include <stdexcept>
#include <string>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Value.h>
#include <p9/ast/expr/Call.hh>
#include <p9/ast/expr/Function.hh>
#include <p9/ast/expr/Number.hh>
#include <p9/ast/stmt/If.hh>
#include <p9/ast/stmt/Return.hh>
#include <p9/ast/Expression.hh>
#include <p9/ast/Statement.hh>
#include <p9/engine/CodeGenerator.hh>
#include <p9/engine/GenerationEngine.hh>
#include <p9/engine/Value.hh>
#include <p9/lexer/Lexer.hh>
#include <p9/parser/Parser.hh>

int main( void )
{
    std::istream & istream = std::cin;

    std::istreambuf_iterator< char > eos;
    std::string s( std::istreambuf_iterator< char >( istream ), eos );

    p9::lexer::Lexer lexer( s.c_str( ), s.length( ) );
    p9::parser::Parser parser( lexer );
    p9::ast::Statement * ast = parser.exec( );

    p9::engine::GenerationEngine generationEngine( "stdin" );
    p9::engine::CodeGenerator codeGenerator( generationEngine );
    llvm::Function * program = codeGenerator.codegen( *ast );

    generationEngine.module( ).dump( );

    std::string errString;
    llvm::InitializeNativeTarget();
    llvm::ExecutionEngine * executionEngine = llvm::EngineBuilder( &( generationEngine.module( ) ) ).setErrorStr( &( errString ) ).create( );
    if ( ! executionEngine ) throw std::runtime_error( errString );

    void * programPtr = executionEngine->getPointerToFunction( program );
    auto callableProgramPtr = reinterpret_cast< p9::engine::Value * (*)( void ) >( programPtr );
    p9::engine::Value * result = callableProgramPtr( );

    switch ( result->type ) {

        case p9::engine::Value::Type::Function:
            std::cout << "Returned a function" << std::endl;
        break;

        case p9::engine::Value::Type::Double:
            std::cout << "Returned a double" << std::endl;
            std::cout << result->doubleData.value << std::endl;
        break;

    }

    return 0;
}
