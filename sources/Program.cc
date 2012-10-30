#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

#include <castel/ast/Statement.hh>
#include <castel/builder/Context.hh>
#include <castel/builder/FunctionBuilder.hh>
#include <castel/lexer/Lexer.hh>
#include <castel/parser/Parser.hh>
#include <castel/runtime/boxes/Class.hh>
#include <castel/runtime/boxes/Bool.hh>
#include <castel/runtime/boxes/Dict.hh>
#include <castel/runtime/boxes/Function.hh>
#include <castel/runtime/boxes/List.hh>
#include <castel/runtime/boxes/Null.hh>
#include <castel/runtime/boxes/Number.hh>
#include <castel/runtime/boxes/String.hh>
#include <castel/runtime/boxes/Undefined.hh>
#include <castel/runtime/Box.hh>
#include <castel/utils/LLVMExtensions.hh>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Function.h>
#include <llvm/Value.h>

#include "Program.hh"

std::string Program::fetch( std::string const & source )
{
    std::ifstream istream( source );
    std::istreambuf_iterator< char > eos;
    return std::string( std::istreambuf_iterator< char >( istream ), eos );
}

castel::ast::Statement * Program::parse( std::string const & code )
{
    castel::lexer::Lexer lexer( code.c_str( ), code.length( ) );
    castel::parser::Parser parser( lexer );
    return parser.exec( );
}

llvm::Function * Program::build( castel::builder::Context & context, castel::ast::Statement * statements )
{
    llvm::Function * llvmProgram = context.declareGlobalFunction< castel::runtime::boxes::Function::Signature >( "main", llvm::Function::ExternalLinkage );
    llvm::BasicBlock * llvmProgramEntry = llvm::BasicBlock::Create( context.llvmContext( ), "", llvmProgram );
    context.irBuilder( ).SetInsertPoint( llvmProgramEntry );

    llvm::Value * llvmFunctionBox = castel::builder::FunctionBuilder( "main" )
        .statements( statements )
    .create( context );

    llvm::Value * llvmZero = llvm::ConstantInt::get( context.llvmContext( ), llvm::APInt( 32, 0 ) );
    llvm::Value * llvmNull = llvm::ConstantPointerNull::get( llvm::TypeBuilder< castel::runtime::Box **, false >::get( context.llvmContext( ) ) );

    llvm::Value * llvmReturnedValue = context.irBuilder( ).CreateCastelCall( "castel_operatorCall", llvmFunctionBox, llvmZero, llvmNull );
    context.irBuilder( ).CreateRet( llvmReturnedValue );

    context.llvmModule( ).dump( );
    llvm::verifyModule( context.llvmModule( ) );

    return llvmProgram;
}

castel::runtime::Box * Program::call( castel::builder::Context & context, llvm::Function * llvmProgram )
{
    llvm::InitializeNativeTarget( );

    std::string errString;
    llvm::ExecutionEngine * executionEngine = llvm::EngineBuilder( &( context.llvmModule( ) ) )
        .setErrorStr( &( errString ) )
    .create( );

    if ( ! executionEngine )
        throw std::runtime_error( errString );

    void * programPtr = executionEngine->getPointerToFunction( llvmProgram );
    auto callableProgramPtr = reinterpret_cast< castel::runtime::Box * ( * )( void ) >( programPtr );
    return callableProgramPtr( );
}

int Program::run( int argc, char ** argv )
{
    std::string content = this->fetch( argv[ 1 ] ? argv[ 1 ] : "/dev/stdin" );
    castel::ast::Statement * statements = this->parse( content );

    castel::builder::Context context( this->fetch( argv[ 1 ] ? argv[ 1 ] : "$_stdin" ) );

    llvm::Function * llvmProgram = this->build( context, statements );
    castel::runtime::Box * box = this->call( context, llvmProgram );

    if ( dynamic_cast< castel::runtime::boxes::Class * >( box ) )
        std::cout << "Returned a class" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Function * >( box ) )
        std::cout << "Returned a function" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Number * >( box ) )
        std::cout << "Returned a number (" << static_cast< castel::runtime::boxes::Number * >( box )->value( ) << ")" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Bool * >( box ) )
        std::cout << "Returned a boolean (" << static_cast< castel::runtime::boxes::Bool * >( box )->value( ) << ")" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::String * >( box ) )
        std::cout << "Return a string (" << static_cast< castel::runtime::boxes::String * >( box )->value( ) << ")" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Null * >( box ) )
        std::cout << "Returned a null value" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Undefined * >( box ) )
        std::cout << "Returned an undefined value" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::Dict * >( box ) )
        std::cout << "Returned a dict" << std::endl;

    if ( dynamic_cast< castel::runtime::boxes::List * >( box ) )
        std::cout << "Returned a list" << std::endl;

    return 0;
}
