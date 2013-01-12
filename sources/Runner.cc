#include <stdexcept>
#include <string>

#include <castel/runtime/Box.hh>
#include <castel/runtime/Module.hh>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/Support/TargetSelect.h>

#include "Runner.hh"

void Runner::staticInitializer( void )
{
    static bool initialized = false;
    if ( initialized ) return ; initialized = true;

    llvm::InitializeNativeTarget( );
}

castel::runtime::Box * Runner::staticDependencyInitializer( Runner * runner, char const * global )
{
    return runner->mGlobals[ global ]( );
}

Runner::Runner( llvm::Module * module )
    : mModule( module )
{
    Runner::staticInitializer( );
}

castel::runtime::Box * Runner::operator()( void )
{
    std::string errString;

    llvm::ExecutionEngine * executionEngine = llvm::EngineBuilder( mModule )
        .setErrorStr( &( errString ) )
    .create( );

    if ( ! executionEngine )
        throw std::runtime_error( errString );

    for ( auto & global : mGlobals )
        executionEngine->addGlobalMapping( mModule->getFunction( global.first + "_generator" ), reinterpret_cast< void * >( & Runner::staticDependencyInitializer ) );

    void * programPtr = executionEngine->getPointerToFunction( mModule->getFunction( "main" ) );
    return reinterpret_cast< castel::runtime::Module::Signature * >( programPtr )( this );
}
