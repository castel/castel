#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

#include <castel/gen/helper/type.hh>
#include <castel/gen/ModuleBuilder.hh>
#include <castel/runtime/Module.hh>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include "Compiler.hh"
#include "Runner.hh"
#include "Source.hh"

Compiler::Compiler( void )
    : mContext( )
{
    llvm::StructType::create( mContext, "Box" );
}

Runner Compiler::build( Source const & source )
{
    llvm::Module * module = new llvm::Module( source.name( ), mContext );

    #define TOSTRING( X ) #X

    #define CASTEL_FUNCTION( NAME, RETURN, PARAMETERS ) llvm::Function::Create( castel::gen::helper::type< RETURN PARAMETERS >( mContext ), llvm::GlobalValue::ExternalLinkage, TOSTRING( NAME ), module );
        #include <castel/runtime/capi.def>
    #undef CASTEL_FUNCTION

    castel::gen::ModuleBuilder( )
        .globals( mGlobals )
        .statements( source.parse( ) )
    .build( mContext, module );

    //module->print( llvm::outs( ), nullptr );
    llvm::verifyModule( * module );

    return Runner( module );
}
