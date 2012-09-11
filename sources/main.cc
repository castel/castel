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
#include <castel/builder/Context.hh>
#include <castel/builder/CodeGenerator.hh>
#include <castel/lexer/Lexer.hh>
#include <castel/parser/Parser.hh>
#include <castel/runtime/Box.hh>
#include <castel/runtime/Function.hh>
#include <castel/runtime/Number.hh>

int main( int argc, char ** argv )
{
    char const * source = argc == 1 ? "/dev/stdin" : argv[ 1 ];
    std::ifstream istream( source );

    std::istreambuf_iterator< char > eos;
    std::string s( std::istreambuf_iterator< char >( istream ), eos );

    castel::lexer::Lexer lexer( s.c_str( ), s.length( ) );
    castel::parser::Parser parser( lexer );
    castel::ast::Statement * ast = parser.exec( );

    castel::builder::Context context( "stdin" );
    castel::builder::CodeGenerator codeGenerator( context );
    llvm::Function * program = codeGenerator.codegen( * ast );

    std::string errString;
    llvm::InitializeNativeTarget( );
    llvm::ExecutionEngine * executionEngine = llvm::EngineBuilder( &( context.llvmModule( ) ) ).setErrorStr( &( errString ) ).create( );
    if ( ! executionEngine ) throw std::runtime_error( errString );

    void * programPtr = executionEngine->getPointerToFunction( program );
    auto callableProgramPtr = reinterpret_cast< castel::runtime::Box * (*)( unsigned int, castel::runtime::Box ** ) >( programPtr );
    castel::runtime::Box * result = callableProgramPtr( 0, nullptr );

    if ( dynamic_cast< castel::runtime::Function * >( result ) )
        std::cout << "Returned a function" << std::endl;

    if ( dynamic_cast< castel::runtime::Number * >( result ) )
        std::cout << "Returned a number (" << static_cast< castel::runtime::Number * >( result )->number( ) << ")" << std::endl;

    return 0;
}
