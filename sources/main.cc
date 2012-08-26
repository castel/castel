#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Value.h>
#include <castel/ast/expr/Call.hh>
#include <castel/ast/expr/Function.hh>
#include <castel/ast/expr/Number.hh>
#include <castel/ast/stmt/If.hh>
#include <castel/ast/stmt/Return.hh>
#include <castel/ast/Expression.hh>
#include <castel/ast/Statement.hh>
#include <castel/engine/CodeGenerator.hh>
#include <castel/engine/GenerationEngine.hh>
#include <castel/engine/Value.hh>
#include <castel/lexer/Lexer.hh>
#include <castel/parser/Parser.hh>

int main( int argc, char ** argv )
{
    std::ifstream istream( argc == 1 ? "/dev/stdin" : argv[ 1 ] );

    std::istreambuf_iterator< char > eos;
    std::string s( std::istreambuf_iterator< char >( istream ), eos );

    castel::lexer::Lexer lexer( s.c_str( ), s.length( ) );
    castel::parser::Parser parser( lexer );
    castel::ast::Statement * ast = parser.exec( );

    castel::engine::GenerationEngine generationEngine( "stdin" );
    castel::engine::CodeGenerator codeGenerator( generationEngine );
    llvm::Function * program = codeGenerator.codegen( *ast );

    llvm::verifyModule( generationEngine.module( ) );

    std::string errString;
    llvm::InitializeNativeTarget();
    llvm::ExecutionEngine * executionEngine = llvm::EngineBuilder( &( generationEngine.module( ) ) ).setErrorStr( &( errString ) ).create( );
    if ( ! executionEngine ) throw std::runtime_error( errString );

    void * programPtr = executionEngine->getPointerToFunction( program );
    auto callableProgramPtr = reinterpret_cast< castel::engine::Value * (*)( ) >( programPtr );
    castel::engine::Value * result = callableProgramPtr( );

    switch ( result->type ) {

        case castel::engine::Value::Type::Function:
            std::cout << "Returned a function" << std::endl;
        break;

        case castel::engine::Value::Type::Double:
            std::cout << "Returned a double" << std::endl;
            std::cout << result->doubleData.value << std::endl;
        break;

    }

    return 0;
}
