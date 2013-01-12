#include <stdexcept>
#include <string>

#include <castel/gen/helper/type.hh>
#include <castel/gen/ModuleBuilder.hh>
#include <llvm/Analysis/Verifier.h>

#include "Compiler.hh"
#include "Runner.hh"
#include "Source.hh"

Compiler::Compiler( void )
    : mContext( )
{
    llvm::StructType::create( mContext, "Box" );
}

Runner Compiler::build( Source * source )
{
    llvm::Module * module = new llvm::Module( source->name( ), mContext );

    #define TOSTRING( X ) #X
    #define CASTEL_FUNCTION( NAME, RETURN, PARAMETERS ) llvm::Function::Create( castel::gen::helper::type< RETURN PARAMETERS >( mContext ), llvm::GlobalValue::ExternalLinkage, TOSTRING( NAME ), module );
    #include <castel/runtime/capi.def>
    #undef CASTEL_FUNCTION

    castel::gen::ModuleBuilder( )
        .statements( source->parse( ) )
    .build( mContext, module );

    llvm::verifyModule( * module );

    return Runner( module );
}
